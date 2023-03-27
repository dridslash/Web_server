/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Eyjafjörður.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 16:54:59 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/27 10:54:54 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server_Eyjafjörður.hpp"

//--------------STATIC_VARIABLES------------

Config Server_Eyjafjörður::conf;

int Server_Eyjafjörður::kq = kqueue();

int Server_Eyjafjörður::reuse = 1;

//------------------------------------------

//--------------STATIC_METHODES------------

Server_Eyjafjörður* Server_Eyjafjörður::Draupnir(){
    return (new Server_Eyjafjörður());
}

//-----------------------------------------

Server_Eyjafjörður::Server_Eyjafjörður(){}

Server_Eyjafjörður::~Server_Eyjafjörður(){}

Server_Eyjafjörður::Server_Eyjafjörður(int sk,const char *port):Server_Socket(sk){
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
    
    // Add_Event_to_queue_ker(Server_Socket,EVFILT_WRITE);

    listeners.insert(Server_Socket);
}

void Server_Eyjafjörður::Upp_ports(char *Config_file){
    conf.ConfigParse(Config_file);
    int MAX_PORTS = conf.Ports.size();
    // listeners.reserve(MAX_PORTS);
   for(std::set<std::string>::iterator it = conf.Ports.begin();it != conf.Ports.end();it++)
        Set_up_listeners((*it).c_str());
}

int Server_Eyjafjörður::multiplexing(){
    char buffer[BUFFER_SIZE];
    struct kevent retrieved_events[MAX_CONNECTIONS];
    Request requestFile;
    Response ResponsePath;
    std::string resp;
    for(;;){
        int n_ev = kevent(Server_Eyjafjörður::kq,NULL,0,retrieved_events,MAX_CONNECTIONS,NULL);
        if (n_ev < 0){
            perror("kevent");
            exit(EXIT_FAILURE);
        }
        for(int i = 0; i < n_ev; i++){
            int fd = static_cast<int>(retrieved_events[i].ident);
                if (retrieved_events[i].filter == EVFILT_READ){
                    // std::cout << "Just Read" << std::endl;
                    if (listeners.find(fd) != listeners.end()){
                // ACCPET CONNECTION
                int client_socket = accept((*listeners.find(fd)),(struct sockaddr *)(&servinfo->ai_addr),
                            reinterpret_cast<socklen_t*>(&servinfo->ai_addrlen));
                            std::cout << "socket accepted" << client_socket << std::endl;
                Client_Smár *client_copy = new Client_Smár(client_socket);
                Add_Client(client_copy);
                if (client_socket < 0){
                    std::cout << "Error in accepting socket\n";
                    CLOSING_SOCKET(client_socket);
                }
                std::cout << "Connetion made"<<std::endl;
                }else{
                    std::cout << "How many clinets -->" << Clients.size() << std::endl;
                         for(std::map<int,Client_Smár*>::iterator it = Clients.begin(); it != Clients.end();it++){
                            std::cout << it->second->Client_Socket << std::endl;
                        ResponsePath.setHost(it->second->Client_Ip_Port_Connected.first);
                        std::stringstream port_string;
                        port_string << it->second->Client_Ip_Port_Connected.second;
                        // std::cout << port_string.str() << std::endl;
					    ResponsePath.setPort(port_string.str());
                        if (recv(it->second->Client_Socket,buffer,BUFFER_SIZE,0) == EAGAIN || recv(it->second->Client_Socket,buffer,BUFFER_SIZE,0) == EWOULDBLOCK){
                            perror("recv");
                            exit(EXIT_FAILURE);
                        }
                        // std::cout << buffer << std::endl;
                        it->second->temp_req = buffer;
                         //CALL PARSE REQUEST METHOD->

                            requestFile.RequestParse(it->second->temp_req);

                        //CALL RESPONSE FORMING METHOD->

                            ResponsePath.ResponseFile(it->second->temp_resp, conf, requestFile);
                    
                            Add_Event_to_queue_ker(fd,EVFILT_WRITE);
                            // std::cout << "loop" << std::endl;
                            // usleep(5000);
                        }
                    }
                }else if (retrieved_events[i].filter == EVFILT_WRITE){
                    std::map<int,Client_Smár*>::iterator it = Clients.begin();
                    while(it != Clients.end()){
                        if (it->second){
                        if (send(it->second->Client_Socket,it->second->temp_resp.c_str(),it->second->temp_resp.size() + 1,0) < 0){
                            perror("send");
                            exit(EXIT_FAILURE);
                        }
                        Delete_Client(it->second);
                        it = Clients.begin();
                    }
                }
            }
        }
    }
}


int Server_Eyjafjörður::Get_Server_Socket()const{
    return (Server_Socket);
}

bool Server_Eyjafjörður::Add_Event_to_queue_ker(int &fd , int filter){
    struct kevent event[1];
    EV_SET(&event[0],fd,filter,EV_ADD,0,0,NULL);
    kevent(kq,event,1,NULL,0,NULL);
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


bool Server_Eyjafjörður::Delete_Event_to_queue_ker(int &fd , int filter){
    struct kevent event[1];
    EV_SET(&event[0],fd,filter,EV_DELETE ,0,0,NULL);
    kevent(kq,event,1,NULL,0,NULL);
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
    Delete_Event_to_queue_ker(client_copy->Client_Socket,EVFILT_READ);
    Delete_Event_to_queue_ker(client_copy->Client_Socket,EVFILT_WRITE);
    CLOSING_SOCKET(client_copy->Client_Socket);
    Clients.erase(client_copy->Client_Socket);
}

 // else{
                //     ResponsePath.setHost((*Clients.find(fd)).second->Client_Ip_Port_Connected.first);
                //     std::stringstream port_string;
                //     port_string << (*Clients.find(fd)).second->Client_Ip_Port_Connected.second;
                //     std::cout << port_string.str() << std::endl;
				// 	ResponsePath.setPort(port_string.str());
                //     if (recv((*Clients.find(fd)).first,buffer,BUFFER_SIZE,0) < 0){
                //         perror("recv");
                //         exit(EXIT_FAILURE);
                //     }   


 // else if (retrieved_events[i].filter == EVFILT_WRITE){
                // // std::cout << "In Write" << std::endl;
                // //SEND RESPONSE NOT COMPLETED 100% ---Prototype--
                //     if (send((*Clients.find(fd)).first,resp.c_str(),resp.size(),0) < 0){
                //         perror("send");
                //         exit(EXIT_FAILURE);
                //     }
                //     // Disable_Event_from_queue_ker(fd,EVFILT_WRITE);
                // }