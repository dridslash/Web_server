#include "Server_Master.hpp"

//--------------STATIC_VARIABLES------------

Config Server_Master::conf;

int Server_Master::reuse = 1;

// Derya_Request Server_Master::Request_parser;

//------------------------------------------

//r-----------------------------------------

//--------------STATIC_METHODES------------

Server_Master* Server_Master::Draupnir(){
    return (new Server_Master());
}

//-----------------------------------------

//r----------------------------------------

Server_Master::Server_Master():event_tracker(0){
    kq = kqueue();
    memset(changelist,0,sizeof(changelist));
    events = (struct kevent *)malloc(sizeof(struct kevent));
    std::ifstream myfile("public/mime.types");
	std::string key, value;
    if ( myfile.is_open() ) {
		while ( myfile ) {
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
	std::string Default = ".html";
    for (std::map<std::string, std::string>::iterator formula = ContentTypes.begin(); formula != ContentTypes.end(); formula++) {
        if (formula->second == resp)
            return formula->first;
    }
	return Default;
}

Server_Master::~Server_Master(){}

Server_Master::Server_Master(int sk,const char *port):Server_Socket(sk){
    memset(changelist,0,sizeof(changelist));
}

void Server_Master::Change_Socket_To_Non_Block(int &fd){
    if (fcntl(fd, F_SETFL ,O_NONBLOCK) < 0){
        perror("FCNTL Error");
        exit(EXIT_FAILURE);
    }
}

void Server_Master::Set_up_listeners(const char *port){
    // PORT = port;

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

    // freeaddrinfo(servinfo);
    
    // Dvergmál("IP Bound");
    
    if (listen(Server_Socket,SOMAXCONN) < 0) {
        perror("Listen Error");
        exit(EXIT_FAILURE);
    }

    // Dvergmál("Server up and listening...");

    Add_Event_to_queue_ker(Server_Socket,EVFILT_READ);
    listeners.insert(Server_Socket);
}

void Server_Master::Upp_ports(char *Config_file){
    if (!Config_file){
        std::cout << "Supply a Config File for the Server to work!" << std::endl;
        exit(EXIT_FAILURE);
    }
    conf.ConfigParse(Config_file);
    int MAX_PORTS = conf.Ports.size();
    PrintStatus();
   for(std::set<std::string>::iterator it = conf.Ports.begin();it != conf.Ports.end();it++)
        Set_up_listeners((*it).c_str());
}

int Server_Master::multiplexing(){
    int j = 0;
    struct timespec timeout;
    timeout.tv_sec = 1;
    timeout.tv_nsec = 0;
    for (;;) {
        int n_ev = kevent(kq,NULL,0,retrieved_events,MAX_CONNECTIONS,&timeout);
        if (n_ev < 0) perror("kevent");
        for (int i = 0; i < n_ev ; i++) {
            if (retrieved_events[i].filter == EVFILT_READ) {
                if (listeners.find(retrieved_events[i].ident) != listeners.end()) {
                //====================== ACCEPT CONNECTION ========================
                int client_socket = accept((*listeners.find(retrieved_events[i].ident)),(struct sockaddr *)(&servinfo->ai_addr),
                    reinterpret_cast<socklen_t*>(&servinfo->ai_addrlen));
                Client_Gymir *client_copy = new Client_Gymir(client_socket);
                if (client_socket < 0) CLOSING_SOCKET(client_socket);
                PrintStatus(client_socket);
                Add_Client(client_copy);
                } else {
                     for(std::map<int,Client_Gymir*>::iterator it = Clients.begin(); it != Clients.end();) {
                        if (Search_in_Events(it->second->Client_Socket,retrieved_events,n_ev) == READ) {
                            if (Fill_Request_State_it(it->second)) DropClient(it);
                            else it++;
                        }
                        else it++;
                    }
                }
            }
            else
                Client_loop(retrieved_events,n_ev);
        }
    }
}

void Server_Master::DropClient(std::map<int,Client_Gymir*>::iterator& it) {
    std::map<int,Client_Gymir*>::iterator ite = it;
    it++;
    Delete_Client(ite->second);
}


void Server_Master::Client_loop(struct kevent *retreived_events, int how_many_events) {
    // std::cout << "Write event" << std::endl;
    for(std::map<int,Client_Gymir*>::iterator it = Clients.begin(); it != Clients.end();){
        if (Search_in_Events(it->second->Client_Socket,retreived_events,how_many_events) == WRITE) {
            if (it->second->IsHeaderSended == 0) {
                it->second->ResponsePath.CheckRequestLine(conf, it->second->Request_parser);
                it->second->ResponsePath.MakeResponse(it->second, *this, it->second->Request_parser);
            }
            if (it->second->ResponsePath.getStatusCode() != -1)
                it->second->ResponsePath.SendResponse(it->second, *this);
            else it->second->ResponsePath.setStatusCode(200);
            if (it->second->Client_Hamr == Response_Completed) DropClient(it);
            else it++;
        }
        else it++;
    }
}

int Server_Master::Fill_Request_State_it(Client_Gymir* Client) {
    char buffer[Max_Reads + 1];
    std::cout << "R_received" << std::endl;
    if (Client->Client_Hamr == Still_Reading_Request) {
        std::cout << "R_received1" << std::endl;
        int R_received = recv(Client->Client_Socket, buffer, Max_Reads, 0);
        std::cout << R_received << std::endl;
        if (R_received <= 0) return 1;
        buffer[R_received] = 0;
        std::string get_when_ended(buffer);
        Client->Request.append(buffer);
        Client->Request_parser.Parse_Request(Client->Request);
        // if (Client->Request.size() == R_received)
        // if (get_when_ended.find("\r\n\r\n") != std::string::npos) {
        //     Client->Client_Hamr = Response_Working_On_Headers;
        //     Client->ResponsePath.CheckRequestLine(Client->Request_parser);
        //     Client->ResponsePath.MakeResponse(Client, Client->Request_parser);
        // }
        if (get_when_ended.find("\r\n") != std::string::npos) {
            PrintStatus(Client->Client_Socket, Client->Request_parser.HTTPMethod.c_str(), Client->Request_parser.Path);
            // std::cout << "/* cursor */" << std::endl;
            Add_Event_to_queue_ker(Client->Client_Socket,EVFILT_WRITE);
            Delete_Event_to_queue_ker(Client->Client_Socket,EVFILT_READ);
            Client->Client_Hamr = Response_Still_Serving;
        }
    }
    return 0;
}

int Server_Master::Get_Server_Socket() const { return (Server_Socket); }

bool Server_Master::Add_Event_to_queue_ker(int fd , int filter){
    EV_SET(&events[0],fd,filter,EV_ADD | EV_ENABLE,0,0,NULL);
    kevent(kq,&events[0],1,NULL,0,NULL);
    return (true);
}


bool Server_Master::Disable_Event_from_queue_ker(int &fd , int filter){
    EV_SET(&events[0],fd,filter,EV_DISABLE ,0,0,NULL);
    kevent(kq,events,1,NULL,0,NULL);
    return (true);
}

bool Server_Master::Enable_Event_from_queue_ker(int &fd , int filter){
    struct kevent event[1];
    EV_SET(&event[0],fd,filter,EV_ENABLE ,0,0,NULL);
    kevent(kq,event,1,NULL,0,NULL);
    return (true);
}


bool Server_Master::Delete_Event_to_queue_ker(int fd , int filter){
    struct kevent event;
    EV_SET(&event, fd, filter, EV_DELETE, 0, 0, NULL);
    kevent(kq, &event, 1, NULL, 0, NULL);
    return (true);
}

void Server_Master::Print_Connection_Info() {
    for(std::map<int,Client_Gymir*>::iterator it = Clients.begin(); it != Clients.end();it++){
        if (it->second)
            std::cout << it->second->Client_Ip_Port_Connected.first << ":" << it->second->Client_Ip_Port_Connected.second << std::endl;
    }
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

bool Server_Master::Check_Hamr_Clients(){
    for(std::map<int,Client_Gymir *>::iterator it = Clients.begin(); it != Clients.end();it++){
        if( it->second->Client_Hamr == Still_Reading_Request)
            return false;
    }
    return true;
}

bool Server_Master::Is_in_write_event(int &client_fd,struct kevent *retrieved_events){
    int n_ev = kevent(Server_Master::kq,NULL,0,retrieved_events,MAX_CONNECTIONS,NULL);
    if (n_ev <  0)
        perror("Kevent");
    for (int i = 0; i < n_ev; i++) {
        if (Clients.find(retrieved_events[i].ident) != Clients.end())
            return true;
    }
    return false;
}

int Server_Master::Search_in_Events(int fd, struct kevent *retrieved_events,int n_ev){
    for(int i = 0 ; i < n_ev ; i++){
        if (fd == retrieved_events[i].ident) {
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