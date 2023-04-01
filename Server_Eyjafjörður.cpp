/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Eyjafjörður.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 16:54:59 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/04/01 13:33:39 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server_Eyjafjörður.hpp"

//--------------STATIC_VARIABLES------------

Config Server_Eyjafjörður::conf;

int Server_Eyjafjörður::kq = kqueue();

int Server_Eyjafjörður::reuse = 1;

Derya_Request Server_Eyjafjörður::Request_parser;

//------------------------------------------

//r-----------------------------------------

//--------------STATIC_METHODES------------

Server_Eyjafjörður* Server_Eyjafjörður::Draupnir(){
    return (new Server_Eyjafjörður());
}

//-----------------------------------------

//r----------------------------------------

Server_Eyjafjörður::Server_Eyjafjörður():event_tracker(0){memset(changelist,0,sizeof(changelist)); events = (struct kevent *)malloc(sizeof(struct kevent));}

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
    // listeners.reserve(MAX_PORTS);
   for(std::set<std::string>::iterator it = conf.Ports.begin();it != conf.Ports.end();it++)
        Set_up_listeners((*it).c_str());
}

int Server_Eyjafjörður::multiplexing(){
    struct timespec timeout;
    timeout.tv_sec = 1;
    timeout.tv_nsec = 0;
    Response ResponsePath;
    for (;;) {
        int n_ev = kevent(Server_Eyjafjörður::kq,NULL,0,retrieved_events,MAX_CONNECTIONS,&timeout);
        std::cout << n_ev << std::endl;
        if (n_ev < 0){
            perror("kevent");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < n_ev ; i++) {
            // unsigned long fd = static_cast<int>(retrieved_events[i].ident);
                if (retrieved_events[i].filter == EVFILT_READ && listeners.find(retrieved_events[i].ident) != listeners.end()){
                    //====================== ACCPET CONNECTION ========================
                    int client_socket = accept((*listeners.find(retrieved_events[i].ident)),(struct sockaddr *)(&servinfo->ai_addr),
                        reinterpret_cast<socklen_t*>(&servinfo->ai_addrlen));
                    std::cout << "socket accepted --> " << client_socket << std::endl;
                    Client_Smár *client_copy = new Client_Smár(client_socket);
                    Add_Client(client_copy);
                    if (client_socket < 0) {
                        std::cout << "Error in accepting socket\n";
                        CLOSING_SOCKET(client_socket);
                    }
                    // std::cout << event_tracker << std::endl;
                    std::cout << "Connetion made"<<std::endl;
                    // Delete_Event_to_queue_ker(*(listeners.find(fd)),EVFILT_READ);
                    // Add_Event_to_queue_ker_witout_clear(*(listeners.find(fd)),EVFILT_READ);
            }
            else {
                for(std::map<int,Client_Smár*>::iterator it = Clients.begin(); it != Clients.end();) {
                    ResponsePath.setHost(it->second->Client_Ip_Port_Connected.first);
                    std::stringstream port_string;
                    port_string << it->second->Client_Ip_Port_Connected.second;
                    ResponsePath.setPort(port_string.str());
                    if(Search_in_Events(it->second->Client_Socket,retrieved_events,n_ev) == READ) {
                        std::cout << "=================== Read Event =============================" << std::endl;
                        Fill_Request_State_it(it->second, ResponsePath);
                        it++;
                    }
                    else if (Search_in_Events(it->second->Client_Socket,retrieved_events,n_ev) == WRITE) {
                       std::cout << "=================== Write Event =============================" << std::endl;
                       if (it->second->Client_Hamr == Response_Still_Serving) {
                            if (it->second->IsHeaderSended == 0) {
                                ResponsePath.CheckRequestLine(conf, Request_parser);
                                ResponsePath.MakeResponse(conf, Request_parser);
                            }
                            if (ResponsePath.SendData(it->second)) {
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
                        //    Send_Response_State_It(it->second, ResponsePath);
                }
            }
        }
    }
}

bool Server_Eyjafjörður::Add_Event_to_ENDOF(int &fd , int filter){
    struct kevent event[1];
    EV_SET(&event[0],fd,filter,EV_ADD | EV_EOF,0,0,NULL);
    kevent(kq,event,1,NULL,0,NULL);
    return (true);
}

int Server_Eyjafjörður::Get_Server_Socket()const{
    return (Server_Socket);
}

bool Server_Eyjafjörður::Add_Event_to_queue_ker(int fd , int filter){
    // if (changelist[event_tracker].ident == 0){
    //     std::cout << "zero here" << std::endl;

    EV_SET(&events[0],fd,filter,EV_ADD,0,0,NULL);
    kevent(kq,&events[0],1,NULL,0,NULL);
    // event_tracker++;
    // }
    return (true);
}

bool Server_Eyjafjörður::Add_Event_to_queue_ker_witout_clear(int fd , int filter){
    // struct kevent event;
    // EV_SET(&event,fd,filter,EV_ADD,0,0,NULL);
    // kevent(kq,&event,1,NULL,0,NULL);
    if (changelist[event_tracker].ident == 0){
        std::cout << "zero here" << std::endl;
    EV_SET(&changelist[event_tracker],fd,filter,EV_ADD,0,0,NULL);
    kevent(kq,&changelist[event_tracker],1,retrieved_events,0,NULL);
    event_tracker++;
    }
    return (true);
}

bool Server_Eyjafjörður::Disable_Event_from_queue_ker(int &fd , int filter){
    struct kevent event[1];
    EV_SET(&event[0],fd,filter,EV_DISABLE ,0,0,NULL);
    kevent(kq,event,1,NULL,0,NULL);
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


void Server_Eyjafjörður::trigger_user_event(){
	struct kevent events[1];
	EV_SET(&events[0], 1234, EVFILT_USER, 0, NOTE_TRIGGER, 0, NULL);
	kevent(kq, events, 1, NULL, 0, NULL);
}

bool Server_Eyjafjörður::Add_EVENT_USER(){
    struct kevent event[1];
    EV_SET(&event[0], 1234, EVFILT_USER, EV_ADD | EV_ENABLE | EV_CLEAR,0, 0, NULL);
    kevent(kq,event,1,NULL,0,NULL);
    return (true);
}


void Server_Eyjafjörður::Delete_Client(Client_Smár *client_copy){
    Delete_Event_to_queue_ker(client_copy->Client_Socket,EVFILT_READ);
    Delete_Event_to_queue_ker(client_copy->Client_Socket,EVFILT_WRITE);
    CLOSING_SOCKET(client_copy->Client_Socket);
    Clients.erase(client_copy->Client_Socket);
}

bool Server_Eyjafjörður::Check_Hamr_Clients(){
    for(std::map<int,Client_Smár *>::iterator it = Clients.begin(); it != Clients.end();it++){
        if(it->second->Client_Hamr == Still_Reading_Request)
            return false;
    }
    return true;
}


void Server_Eyjafjörður::Fill_Request_State_it(Client_Smár* client_request_state, Response& ResponsePath){
    std::cout << "=================== Read Event =============================" << std::endl;
    if (client_request_state->Client_Hamr == Still_Reading_Request) {
        int R_received = recv(client_request_state->Client_Socket,
            client_request_state->Request + client_request_state->Bytes_received,Max_Reads - client_request_state->Bytes_received,0);
        // if (R_received  == EAGAIN || R_received  == EWOULDBLOCK || R_received < 0) {
        //     perror("recv");
        //     exit(EXIT_FAILURE);
        // }
        if (R_received <= 0){
            Delete_Client(client_request_state);
            std::cout << "Client Droped" << std::endl;
        }
        client_request_state->Bytes_received += R_received;
        client_request_state->Request[client_request_state->Bytes_received] = 0;
        std::cout << client_request_state->Request << std::endl;
        std::string get_when_ended(client_request_state->Request);
        Request_parser.Parse_Request(client_request_state->Request);
        if (get_when_ended.find("\r\n\r\n") != std::string::npos)
            client_request_state->Client_Hamr = Request_Completed;
    }
    if (client_request_state->Client_Hamr == Request_Completed) {
        Add_Event_to_queue_ker(client_request_state->Client_Socket,EVFILT_WRITE);
        client_request_state->Client_Hamr = Response_Still_Serving;
    }
}


// void Server_Eyjafjörður::Send_Response_State_It(Client_Smár* client_request_state, Response& ResponsePath){
//     int S_sended = 0;
//         std::cout << "=================== Write Event =============================" << std::endl;
        
//         //============================ RESPONSE TO BE REFACTORED =======================
//         if (client_request_state->Client_Hamr == Response_Still_Serving) {
//         ResponsePath.ResponseFile(client_request_state->temp_resp, conf, Request_parser);
//         if (client_request_state->Bytes_Sended != client_request_state->temp_resp.size()) {
//             int Size;
//             (Max_Writes - client_request_state->Bytes_Sended > client_request_state->temp_resp.size()) ? Size = client_request_state->temp_resp.size() : Size = Max_Writes - client_request_state->Bytes_Sended;
//             S_sended = send(client_request_state->Client_Socket, client_request_state->temp_resp.c_str() + client_request_state->Bytes_Sended, Size, 0);
//             if (S_sended < 0) {
//                 perror("send");
//                 exit(EXIT_FAILURE);
//             }
//             client_request_state->Bytes_Sended += S_sended;
//         }
//         else
//             Delete_Client(client_request_state);
//         }
// }

bool Server_Eyjafjörður::Is_in_write_event(int &client_fd,struct kevent *retrieved_events){
    int n_ev = kevent(Server_Eyjafjörður::kq,NULL,0,retrieved_events,MAX_CONNECTIONS,NULL);
    if (n_ev <  0){
        perror("Kevent");
    }
        for(int i = 0; i < n_ev; i++){
            if (Clients.find(retrieved_events[i].ident) != Clients.end())
                return true;
        }
    return false;
}

int Server_Eyjafjörður::Search_in_Events(int fd, struct kevent *retrieved_events,int n_ev){
    for(int i = 0 ; i < n_ev ; i++){
        if (fd == retrieved_events[i].ident){
            if (retrieved_events[i].filter == EVFILT_READ)
                return READ;
            else
                if (retrieved_events[i].filter == EVFILT_WRITE)
                return WRITE;
        }
    }
    return -1;
    
    // events_register.find()
}