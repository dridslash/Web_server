/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client_Gymir.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 16:59:52 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/31 09:25:00 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client_Gymir.hpp"

//--------------STATIC_METHODES------------

Client_Gymir* Client_Gymir::Draupnir_Smár(int sockcl) { return (new Client_Gymir(sockcl)); }

//-----------------------------------------

Client_Gymir::Client_Gymir():Client_Hamr(Still_Reading_Request), Bytes_received(0), Bytes_Sended(0), IsHeaderSended(false), IsCGI(false) {
    
}

Client_Gymir::Client_Gymir(int sockcl):Client_Socket(sockcl), Client_Hamr(Still_Reading_Request), Bytes_received(0), Bytes_Sended(0), IsHeaderSended(false), IsCGI(false) {
    
}

Client_Gymir::~Client_Gymir(){
    
}

void Client_Gymir::Set_up_ip_port(){
    memset(&Own_addr,0,sizeof(Own_addr));
    int client_addlen = sizeof(Own_addr);
    getsockname(Client_Socket,(struct sockaddr*)&Own_addr,(socklen_t*)&client_addlen);
    Client_Ip_Port_Connected.first = inet_ntoa(Own_addr.sin_addr);
    Client_Ip_Port_Connected.second = ntohs(Own_addr.sin_port);
    ResponsePath.setHost(Client_Ip_Port_Connected.first);
    std::stringstream port_string;
    port_string << Client_Ip_Port_Connected.second;
    ResponsePath.setPort(port_string.str());
}

// void Client_Gymir::Fill_Request_State_it(){
//     if (Client_Hamr == Still_Reading_Request){
//     int R_received = recv(Client_Socket,Request + Bytes_received,1,0);
//     if (R_received  == EAGAIN || R_received  == EWOULDBLOCK || R_received < 0){
//         perror("recv");
//         exit(EXIT_FAILURE);
//     }
//     std::cout << R_received << std::endl;
//     Bytes_received += R_received;
//     Request[Bytes_received] = 0;
//     std::string get_when_ended(Request);
//     if (get_when_ended.find("\r\n\r\n") != std::string::npos){
//             Client_Hamr = Request_Completed;
//         }
//     }
// }