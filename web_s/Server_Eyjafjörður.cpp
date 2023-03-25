/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Eyjafjörður.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 16:54:59 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/25 15:59:11 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server_Eyjafjörður.hpp"

//--------------STATIC_VARIABLES------------

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

Server_Eyjafjörður::Server_Eyjafjörður(int sk,const char *port):Server_Socket(sk),PORT(port){
}

void Server_Eyjafjörður::Change_Socket_To_Non_Block(int &fd){
        if (fcntl(fd,O_NONBLOCK) < 0){
        perror("FCNTL Error");
        exit(EXIT_FAILURE);
    }
}

int Server_Eyjafjörður::Set_up_Server(const char *port){
    PORT = port;

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
    Server_Socket = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);
    if (setsockopt(Server_Socket,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse)) < 0){
        perror("Setsockopt Error");
        exit(EXIT_FAILURE);
    }
    
    Change_Socket_To_Non_Block(Server_Socket);
    
    if (bind(Server_Socket,(struct sockaddr*)servinfo->ai_addr,servinfo->ai_addrlen) < 0){
        perror("Bind Error");
        exit(EXIT_FAILURE);
    }
    
    freeaddrinfo(servinfo);
    
    Dvergmál("IP Bound");
    
    if (listen(Server_Socket,SOMAXCONN) < 0){
        perror("Listen Error");
        exit(EXIT_FAILURE);
    }
    
    Dvergmál("Server up and listening...");

    Add_Event_to_queue_ker(Server_Socket,EVFILT_READ);
    Add_Event_to_queue_ker(Server_Socket,EVFILT_WRITE);

    return (0);
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

// bool Delete_Event_to_queue_ker(int &fd , int filter);

void Server_Eyjafjörður::Print_Connection_Info(){
    for(std::map<int,Client_Smár*>::iterator it = Clients.begin(); it != Clients.end();it++){
        if (it->second){
        std::cout << it->second->Client_Ip_Port_Connected.first << ":" << it->second->Client_Ip_Port_Connected.second << std::endl;
        }
    }
}

void Server_Eyjafjörður::Add_Client(int &fd){
    Client_Smár *client_copy = Client_Smár::Draupnir_Smár(fd);
    Add_Event_to_queue_ker(client_copy->Client_Socket,EVFILT_READ);
    Add_Event_to_queue_ker(client_copy->Client_Socket,EVFILT_WRITE);
    client_copy->Set_up_ip_port();
    Clients.insert(std::make_pair(client_copy->Client_Socket,client_copy));
}