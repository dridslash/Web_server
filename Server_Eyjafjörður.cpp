#include "Server_Eyjafjörður.hpp"

//--------------STATIC_VARIABLES------------

Config Server_Eyjafjörður::conf;

int Server_Eyjafjörður::kq = kqueue();

int Server_Eyjafjörður::reuse = 1;

// Derya_Request Server_Eyjafjörður::Request_parser;

//------------------------------------------

//r-----------------------------------------

//--------------STATIC_METHODES------------

Server_Eyjafjörður* Server_Eyjafjörður::Draupnir(){
    return (new Server_Eyjafjörður());
}

//-----------------------------------------

//r----------------------------------------

Server_Eyjafjörður::Server_Eyjafjörður():event_tracker(0){
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

std::string Server_Eyjafjörður::getContentType(const char* resp) {
	std::string Default = "application/octet-stream";
	std::map<std::string, std::string>::iterator formula = ContentTypes.find(strrchr(resp, '.'));
	if (formula != ContentTypes.end())
		return formula->second;
	return Default;
}

Server_Eyjafjörður::~Server_Eyjafjörður(){}

Server_Eyjafjörður::Server_Eyjafjörður(int sk,const char *port):Server_Socket(sk){
    memset(changelist,0,sizeof(changelist));
}

void Server_Eyjafjörður::Change_Socket_To_Non_Block(int &fd){
    if (fcntl(fd, F_SETFL ,O_NONBLOCK) < 0){
        perror("FCNTL Error");
        exit(EXIT_FAILURE);
    }
}

void Server_Eyjafjörður::Set_up_listeners(const char *port){
    // PORT = port;

    memset(&hints, 0, sizeof(hints));
    
    // hints.ai_family = AF_UNSPEC;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // const char *port = "8080";
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
    
    Dvergmál("IP Bound");
    
    if (listen(Server_Socket,SOMAXCONN) < 0){
        perror("Listen Error");
        exit(EXIT_FAILURE);
    }
    
    Dvergmál("Server up and listening...");

    Add_Event_to_queue_ker(Server_Socket,EVFILT_READ);

    listeners.insert(Server_Socket);
}

void Server_Eyjafjörður::Upp_ports(char *Config_file){
    if (!Config_file){
        std::cout << "Supply a Config File for the Server to work!" << std::endl;
        exit(EXIT_FAILURE);
    }
    conf.ConfigParse(Config_file);
    int MAX_PORTS = conf.Ports.size();
    std::cout << "Initializing Servers..." << std::endl;
   for(std::set<std::string>::iterator it = conf.Ports.begin();it != conf.Ports.end();it++)
        Set_up_listeners((*it).c_str());
}

int Server_Eyjafjörður::multiplexing(){
    struct timespec timeout;
    timeout.tv_sec = 1;
    timeout.tv_nsec = 0;
    for (;;) {
        int n_ev = kevent(Server_Eyjafjörður::kq,NULL,0,retrieved_events,MAX_CONNECTIONS,&timeout);
        if (n_ev < 0){
            perror("kevent");
        }
        for (int i = 0; i < n_ev ; i++) {
            if (retrieved_events[i].filter == EVFILT_READ){
                if (listeners.find(retrieved_events[i].ident) != listeners.end()){
                //====================== ACCEPT CONNECTION ========================
                int client_socket = accept((*listeners.find(retrieved_events[i].ident)),(struct sockaddr *)(&servinfo->ai_addr),
                    reinterpret_cast<socklen_t*>(&servinfo->ai_addrlen));
                Client_Smár *client_copy = new Client_Smár(client_socket);
                if (client_socket < 0)
                    CLOSING_SOCKET(client_socket);
                std::cout << "New Connection, Assigned Socket " << client_socket << std::endl;
                Add_Client(client_copy);
                } else {
                     for(std::map<int,Client_Smár*>::iterator it = Clients.begin(); it != Clients.end();){
                        if (Search_in_Events(it->second->Client_Socket,retrieved_events,n_ev) == READ) {
                            if (Fill_Request_State_it(it->second)) {
                                std::map<int,Client_Smár*>::iterator ite = it;
                                it++;
                                std::cout << "Delete in Read Event" << std::endl;
                                Delete_Client(ite->second);
                            }
                            else
                                it++;
                        }
                        else
                            it++;
                    }
                }
            }
            else
                Client_loop(retrieved_events,n_ev);
        }
    }
}


void Server_Eyjafjörður::Client_loop(struct kevent *retreived_events, int how_many_events){
    for(std::map<int,Client_Smár*>::iterator it = Clients.begin(); it != Clients.end();){
        if (Search_in_Events(it->second->Client_Socket,retreived_events,how_many_events) == WRITE) {
            if (it->second->IsHeaderSended == 0) {
                it->second->ResponsePath.CheckRequestLine(conf, it->second->Request_parser);
                it->second->ResponsePath.MakeResponse(it->second, conf, it->second->Request_parser);
            }
            it->second->ResponsePath.SendData(it->second, *this);
            if (it->second->Client_Hamr == Response_Completed) {
                std::map<int,Client_Smár*>::iterator ite = it;
                it++;
                Delete_Client(ite->second);
            }
            else
                it++;
        }
        else
            it++;
    }
}

int Server_Eyjafjörður::Get_Server_Socket()const{
    return (Server_Socket);
}

bool Server_Eyjafjörður::Add_Event_to_queue_ker(int fd , int filter){
    EV_SET(&events[0],fd,filter,EV_ADD ,0,0,NULL);
    kevent(kq,&events[0],1,NULL,0,NULL);
    return (true);
}


bool Server_Eyjafjörður::Disable_Event_from_queue_ker(int &fd , int filter){
    EV_SET(&events[0],fd,filter,EV_DISABLE ,0,0,NULL);
    kevent(kq,events,1,NULL,0,NULL);
    return (true);
}

bool Server_Eyjafjörður::Enable_Event_from_queue_ker(int &fd , int filter){
    struct kevent event[1];
    EV_SET(&event[0],fd,filter,EV_ENABLE ,0,0,NULL);
    kevent(kq,event,1,NULL,0,NULL);
    
    return (true);
}


bool Server_Eyjafjörður::Delete_Event_to_queue_ker(int fd , int filter){
    struct kevent event;
    EV_SET(&event, fd, filter, EV_DELETE, 0, 0, NULL);
    kevent(kq, &event, 1, NULL, 0, NULL);
    return (true);
}

void Server_Eyjafjörður::Print_Connection_Info(){
    for(std::map<int,Client_Smár*>::iterator it = Clients.begin(); it != Clients.end();it++){
        if (it->second){
        std::cout << it->second->Client_Ip_Port_Connected.first << ":" << it->second->Client_Ip_Port_Connected.second << std::endl;
        }
    }
}

void Server_Eyjafjörður::Add_Client(Client_Smár *client_copy){
    Change_Socket_To_Non_Block(client_copy->Client_Socket);
    Add_Event_to_queue_ker(client_copy->Client_Socket,EVFILT_READ);
    client_copy->Set_up_ip_port();
    Clients.insert(std::make_pair(client_copy->Client_Socket,client_copy));
}

void Server_Eyjafjörður::Delete_Client(Client_Smár *client_copy){
    // std::cout << "Client Deleted" << std::endl;
    printf("Closing Connection With Client %d\n", client_copy->Client_Socket);
    Delete_Event_to_queue_ker(client_copy->Client_Socket,EVFILT_READ);
    Delete_Event_to_queue_ker(client_copy->Client_Socket,EVFILT_WRITE);
    CLOSING_SOCKET(client_copy->Client_Socket);
    Clients.erase(client_copy->Client_Socket);
}

bool Server_Eyjafjörður::Check_Status_Of_Clients(){
    for(std::map<int,Client_Smár *>::iterator it = Clients.begin(); it != Clients.end();it++){
        if(it->second->Client_Hamr == Still_Reading_Request)
            return false;
    }
    return true;
}


int Server_Eyjafjörður::Fill_Request_State_it(Client_Smár* client_request_state){
    if (client_request_state->Client_Hamr == Still_Reading_Request){
    int R_received = recv(client_request_state->Client_Socket,
        client_request_state->Request + client_request_state->Bytes_received,Max_Reads - client_request_state->Bytes_received,0);
    if (R_received <= 0){
        return 1;
    }
    client_request_state->Bytes_received += R_received;
    client_request_state->Request[client_request_state->Bytes_received] = 0;
    // std::cout << client_request_state->Request << std::endl;
    std::string get_when_ended(client_request_state->Request);
    client_request_state->Request_parser.Parse_Request(client_request_state->Request);
    if (get_when_ended.find("\r\n\r\n") != std::string::npos){
        printf("\033[0;33mRequest Recived Form Socket %d, Method=<%s>  URI=<%s>\033[0m\n",
        client_request_state->Client_Socket, client_request_state->Request_parser.HTTPMethod.c_str(), client_request_state->Request_parser.Path.c_str());
        // std::cout << "================ Request Complete =================" << std::endl;
        Add_Event_to_queue_ker(client_request_state->Client_Socket,EVFILT_WRITE);
        Disable_Event_from_queue_ker(client_request_state->Client_Socket,EVFILT_READ);
        client_request_state->Client_Hamr = Response_Still_Serving;
        }
    }
    return 0;
}


int Server_Eyjafjörður::Search_in_Events(int fd, struct kevent *retrieved_events,int n_ev){
    for(int i = 0 ; i < n_ev ; i++){
        if (fd == retrieved_events[i].ident){
            if (retrieved_events[i].filter == EVFILT_READ)
                return READ;
            else if (retrieved_events[i].filter == EVFILT_WRITE)
                return WRITE;
        }
    }
    return -1;
}