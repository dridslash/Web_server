/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gymir.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 16:54:59 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/26 12:20:31 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Gymir.hpp"
#define BUFFER_SIZE 1024

// Config Gymir::conf;

// Gymir::Gymir(){}

// Gymir::Gymir(const std::vector<Server_Eyjafjörður*>& fill_servers):Servers(fill_servers){}

// Gymir::~Gymir(){}

// void Gymir::Create_Servers_Acc_Port(int number_of_ports, const char *port){
//         Server_Eyjafjörður *Serv = Server_Eyjafjörður::Draupnir();
//         Serv->Set_up_Server(port);
//         Servers.push_back(Serv);
// }

// int Gymir::Upping_Eyjafjörðurs(char *Config_file){
//     conf.ConfigParse(Config_file);
//     int MAX_PORTS = conf.Ports.size();
//     Servers.reserve(MAX_PORTS);
//    for(std::set<std::string>::iterator it = conf.Ports.begin();it != conf.Ports.end();it++){
//         Create_Servers_Acc_Port(MAX_PORTS, (*it).c_str());
//     }
//     multiplexing(Servers);
//     return (0);
// }

// Server_Eyjafjörður* Gymir::Search_in_Servers(int fd,std::vector<Server_Eyjafjörður*>& Serv_All){
//     for(std::vector<Server_Eyjafjörður>::size_type i = 0;i < Serv_All.size(); i++){
//         if (fd == Serv_All.at(i)->Get_Server_Socket())
//             return (Serv_All.at(i));
//     }
//     return (NULL);
// }

// int Gymir::multiplexing(std::vector<Server_Eyjafjörður*>& Serv_All){
//     // std::string response ("HTTP/1.1 200 OK\r\n"
//     //                     "Server: webserver-c\r\n"
//     //                     "Content-type: text/html\r\n\r\n"
//     //                     "<html>im the server here is you response</html>\r\n");

//     //                     std::string msg_from_client ("HTTP/1.1 200 OK\r\n"
//     //                     "Server: webserver-c\r\n"
//     //                     "Content-type: text/html\r\n\r\n"
//     //                     "<html>im the client here is you response</html>\r\n");
//     std::string wel ("HTTP/1.1 200 OK\r\n\r\n"
//                 "<header>Welcome to the server</header>\r\n");
//     char buffer[BUFFER_SIZE];
//     char k[20];
//     struct kevent retrieved_events[MAX_CONNECTIONS];
//     int client_socket;
//     for(;;){
//         int n_ev = kevent(Server_Eyjafjörður::kq,NULL,0,retrieved_events,MAX_CONNECTIONS,NULL);
//         if (n_ev < 0){
//             perror("kevent");
//             exit(EXIT_FAILURE);
//         }
//         for(int i = 0; i < n_ev; i++){
//             int fd = static_cast<int>(retrieved_events[i].ident);
            
//             Server_Eyjafjörður* copy_server = Search_in_Servers(fd,Serv_All);
//                 if (retrieved_events[i].filter == EVFILT_READ){
//                     std::cout << "here1111" << std::endl;
//                 //     if (copy_server && fd == copy_server->Get_Server_Socket()){
//                 // // ACCPET CONNECTION
//                 // client_socket = accept(copy_server->Get_Server_Socket(),(struct sockaddr *)(&copy_server->servinfo->ai_addr),
//                 //             reinterpret_cast<socklen_t*>(&copy_server->servinfo->ai_addrlen));
                
//                 // copy_server->Add_Client(client_socket);
//                 // if (client_socket < 0){
//                 //     std::cout << "Error in accepting socket\n";
//                 //     CLOSING_SOCKET(client_socket);
//                 // }
//                 // std::cout << "Connetion made"<<std::endl;
//                 // if (recv((*(copy_server->Clients.find(client_socket))).first,buffer,BUFFER_SIZE,0) <= 0){
//                 //     perror("recv");
//                 //     exit(EXIT_FAILURE);
//                 // }
//                 // std::cout << buffer << std::endl;
//                 // }
//              }
//         }
//     }
// }