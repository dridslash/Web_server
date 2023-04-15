#include "Server_Master.hpp"
#include "Client_Gymir.hpp"

Config Server_Master::conf;

int Server_Master::reuse = 1;

Server_Master* Server_Master::Draupnir(){
    return (new Server_Master());
}

Server_Master::Server_Master():event_tracker(0){
    kq = kqueue();
    memset(changelist,0,sizeof(changelist));
    events = (struct kevent *)malloc(sizeof(struct kevent));
    std::ifstream myfile("public/mime.types");
	std::string key, value;
    if ( myfile.is_open() ) {
		while ( !myfile.eof() ) {
			myfile >> value >> key;
			ContentTypes.insert(std::make_pair(key, value));
		}
	}
}

std::string Server_Master::getContentType(const char* resp) {
	std::string Default = "application/octet-stream";
	std::map<std::string, std::string>::iterator formula = ContentTypes.find(strrchr(resp, '.'));
	if (formula != ContentTypes.end())
		return formula->second;
	return Default;
}

std::string Server_Master::getReverseContentType(const char* resp) {
	std::string Default = "";
    for (std::map<std::string, std::string>::iterator formula = ContentTypes.begin(); formula != ContentTypes.end(); formula++) {
        if (formula->second == resp)
            return formula->first;
    }
	return Default;
}

Server_Master::~Server_Master(){}

Server_Master::Server_Master(int sk):Server_Socket(sk){
    memset(changelist,0,sizeof(changelist));
}

void Server_Master::Change_Socket_To_Non_Block(int &fd) {
    if (fcntl(fd, F_SETFL ,O_NONBLOCK) < 0)
        perror("FCNTL Error");
}

void Server_Master::Set_up_listeners(const char *port){
    memset(&hints, 0, sizeof(hints));
    
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int get_add_errno = 0;
    if ((get_add_errno = getaddrinfo(NULL, port, &hints, &servinfo)) != 0){
        std::cerr << (get_add_errno) << std::endl;
    }
    if ((Server_Socket = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol)) < 0){
        perror("Server_Socket");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(Server_Socket,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse)) < 0){
        perror("Setsockopt Error");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(Server_Socket,SOL_SOCKET,SO_NOSIGPIPE,&reuse,sizeof(reuse)) < 0){
        perror("Setsockopt Error");
        exit(EXIT_FAILURE);
    }

    Change_Socket_To_Non_Block(Server_Socket);

    if (bind(Server_Socket,(struct sockaddr*)servinfo->ai_addr,servinfo->ai_addrlen) < 0){
        perror("Bind Error");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(servinfo);
        
    if (listen(Server_Socket,SOMAXCONN) < 0) {
        perror("Listen Error");
        exit(EXIT_FAILURE);
    }
    Add_Event_to_queue_ker(Server_Socket,EVFILT_READ);
    listeners.insert(Server_Socket);
}

void Server_Master::Upp_ports(char *Config_file){
    if (!Config_file){
        std::cout << "Supply a Config File for the Server to work!" << std::endl;
        exit(EXIT_FAILURE);
    }
    struct stat sb;
    if (stat(Config_file, &sb)) {
        std::cout << "Invalid Configuration File" << std::endl;
        exit(EXIT_FAILURE);
    }
    conf.ConfigParse(Config_file);
    PrintStatus();
   for(std::set<std::string>::iterator it = conf.Ports.begin();it != conf.Ports.end();it++)
        Set_up_listeners((*it).c_str());
}

int Server_Master::multiplexing(){
    struct timespec timeout;
    timeout.tv_sec = 1;
    timeout.tv_nsec = 0;
    for (;;) {
        int n_ev = kevent(kq,NULL,0,retrieved_events,MAX_CONNECTIONS,&timeout);
        if (n_ev < 0) perror("kevent");
        for (int i = 0; i < n_ev ; i++) {
            if (retrieved_events[i].filter == EVFILT_READ) {
                if (listeners.find(retrieved_events[i].ident) != listeners.end()){
                    //====================== ACCEPT CONNECTION ========================
                    int client_socket = accept((*listeners.find(retrieved_events[i].ident)), NULL, NULL);
                    Client_Gymir *client_copy = new Client_Gymir(client_socket);
                    if (client_socket < 0) CLOSING_SOCKET(client_socket);
                    PrintStatus(client_socket);
                    Add_Client(client_copy);
                }
                else
                    Reading_Part(n_ev);
            }
            else
                Sending_Part(retrieved_events,n_ev);
        }
    }
}

void Server_Master::Reading_Part(int n_ev) {
    for (std::map<int,Client_Gymir*>::iterator it = Clients.begin(); it != Clients.end();) {
        if (Search_in_Events(it->second->Client_Socket,retrieved_events,n_ev) == READ) {
            if (Fill_Request_State_it(it->second))
                DropClient(it);
            else
                it++;
        }
        else it++;
    }
}

void Server_Master::DropClient(std::map<int,Client_Gymir*>::iterator& it) {
    std::map<int,Client_Gymir*>::iterator ite = it;
    it++;
    Delete_Client(ite->second);
}


void Server_Master::Sending_Part(struct kevent *retreived_events, int how_many_events) {
    for(std::map<int,Client_Gymir*>::iterator it = Clients.begin(); it != Clients.end();){
        if (Search_in_Events(it->second->Client_Socket,retreived_events,how_many_events) == WRITE) {
            if (it->second->IsHeaderSended == 0) {
                it->second->ResponsePath.CheckRequestLine(conf, it->second->Request_parser);
                it->second->ResponsePath.MakeResponse(it->second, *this, it->second->Request_parser);
            }
            if (it->second->ResponsePath.getStatusCode() != -1)
                it->second->ResponsePath.SendResponse(it->second, *this);
            else it->second->ResponsePath.setStatusCode(200);
            if (it->second->Client_Hamr == Response_Completed)
                DropClient(it);
            else it++;
        }
        else it++;
    }
}

int Server_Master::Fill_Request_State_it(Client_Gymir* client_request_state) {
    char buffer[Max_Reads + 1];
    memset(buffer,0,Max_Reads);
    int stat = 0;
    if (client_request_state->Client_Hamr == Still_Reading_Request) {
        client_request_state->returnRead = recv(client_request_state->Client_Socket,buffer, Max_Reads, 0);
        if (client_request_state->returnRead <= 0) {
            if (client_request_state->Request_parser.HTTPMethod.size() && client_request_state->Request_parser.HTTPMethod == "POST")
                PrintStatus(client_request_state->Client_Socket, client_request_state->Request_parser.HTTPMethod.c_str(), client_request_state->Request_parser.Path);
            return 1;
        }
        buffer[client_request_state->returnRead] = 0;
        client_request_state->Request.assign(buffer , buffer + client_request_state->returnRead);
        client_request_state->Request_parser.Parse_Request(*client_request_state,*this);
        stat = client_request_state->Request_parser.stat_method_form.first;
        if (client_request_state->Request_parser.stat_method_form.second == POST && (stat == 200 || stat == 400 || stat == 501)){
            PrintStatus(client_request_state->Client_Socket, client_request_state->Request_parser.HTTPMethod.c_str(), client_request_state->Request_parser.Path);
            client_request_state->ResponsePath.setStatusCode(stat);
            Add_Event_to_queue_ker(client_request_state->Client_Socket,EVFILT_WRITE);
            Disable_Event_from_queue_ker(client_request_state->Client_Socket,EVFILT_READ);
            client_request_state->Client_Hamr = Response_Still_Serving;
        }
        else if ((client_request_state->Request_parser.stat_method_form.second == GET || client_request_state->Request_parser.stat_method_form.second == DELETE) 
                && (stat == 200 || stat == 400 || stat == 501)){
            PrintStatus(client_request_state->Client_Socket, client_request_state->Request_parser.HTTPMethod.c_str(), client_request_state->Request_parser.Path);
            client_request_state->Request = client_request_state->Request_parser.Hold_sliced_Request.first;
            client_request_state->ResponsePath.setStatusCode(stat);
            if (client_request_state->Request.find("\r\n\r\n") != std::string::npos || client_request_state->Request.find("\r\n") != std::string::npos) {
                Add_Event_to_queue_ker(client_request_state->Client_Socket,EVFILT_WRITE);
                Disable_Event_from_queue_ker(client_request_state->Client_Socket,EVFILT_READ);
                client_request_state->Client_Hamr = Response_Still_Serving;
            }
        }
    }
    return 0;
}


bool Server_Master::Add_Event_to_queue_ker(int fd , int filter){
    EV_SET(&events[0], fd, filter, EV_ADD, 0, 0, NULL);
    kevent(kq,&events[0],1,NULL,0,NULL);
    return (true);
}


bool Server_Master::Disable_Event_from_queue_ker(int &fd , int filter){
    EV_SET(&events[0], fd, filter, EV_DISABLE, 0, 0, NULL);
    kevent(kq,events, 1, NULL, 0, NULL);
    return (true);
}

bool Server_Master::Delete_Event_to_queue_ker(int fd , int filter){
    struct kevent event;
    EV_SET(&event, fd, filter, EV_DELETE, 0, 0, NULL);
    kevent(kq, &event, 1, NULL, 0, NULL);
    return (true);
}

void Server_Master::Add_Client(Client_Gymir *client_copy){
    Change_Socket_To_Non_Block(client_copy->Client_Socket);
    Add_Event_to_queue_ker(client_copy->Client_Socket,EVFILT_READ);
    client_copy->Set_up_ip_port();
    Clients.insert(std::make_pair(client_copy->Client_Socket,client_copy));
}

void Server_Master::Delete_Client(Client_Gymir *client_copy){
    PrintStatus(client_copy->Client_Socket, 0, std::string(), 0, 1);
    Delete_Event_to_queue_ker(client_copy->Client_Socket,EVFILT_READ);
    Delete_Event_to_queue_ker(client_copy->Client_Socket,EVFILT_WRITE);
    CLOSING_SOCKET(client_copy->Client_Socket);
    Clients.erase(client_copy->Client_Socket);
    delete client_copy;
}

int Server_Master::Search_in_Events(int fd, struct kevent *retrieved_events,int n_ev){
    for(int i = 0 ; i < n_ev ; i++){
        if (fd == static_cast<int>(retrieved_events[i].ident)) {
            if (retrieved_events[i].filter == EVFILT_READ)
                return READ;
            else if (retrieved_events[i].filter == EVFILT_WRITE)
                return WRITE;
        }
    }
    return -1;
}

void Server_Master::PrintStatus(int fd, const char* HTTPMethod, std::string Path, int StatusCode, int flag) {
    if (HTTPMethod) {
        printf("\033[0;31m     [ Request  ]     [INFO]          Request Recived From Socket %d, Method=<%s>  URI=<%s", fd, HTTPMethod, Path.substr(0, 25).c_str());
        if (Path.size() > 25) printf("...");
        printf(">\n\033[0m");
    }
    else if (StatusCode) {
        printf("\033[0;36m     [ Response ]     [INFO]          Response Sent To Socket %d, Stats=<%d>  Path=<%s", fd, StatusCode, Path.substr(0, 25).c_str());
        if (Path.size() > 25) printf("...");
        printf(">\n\033[0m");
    }
    else if (fd) {
        if (!flag)
            std::cout << "     [Connection]     [INFO]          New Connection, Assigned Socket " << fd << std::endl;
        else
            printf("     [Connection]     [INFO]          Closing Connection With Client %d\n", fd);
    }
    else
        std::cout << "     [  Server  ]     [INFO]          Initializing Servers..." << std::endl;
}