/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client_Smár.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 16:59:52 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/28 18:35:20 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client_Smár.hpp"

//--------------STATIC_METHODES------------

Client_Smár* Client_Smár::Draupnir_Smár(int sockcl){
        return (new Client_Smár(sockcl));
    }

//-----------------------------------------

Client_Smár::Client_Smár(){
    
}

Client_Smár::Client_Smár(int sockcl):Client_Socket(sockcl), Client_Hamr(Still_Reading_Request), Bytes_received(0){
    
}

Client_Smár::~Client_Smár(){
    
}

void Client_Smár::Set_up_ip_port(){
    memset(&Own_addr,0,sizeof(Own_addr));
    int client_addlen = sizeof(Own_addr);
    getsockname(Client_Socket,(struct sockaddr*)&Own_addr,(socklen_t*)&client_addlen);
    Client_Ip_Port_Connected.first = inet_ntoa(Own_addr.sin_addr);
    Client_Ip_Port_Connected.second = ntohs(Own_addr.sin_port);
}

void Client_Smár::Fill_Request_State_it(){
    int R_received = recv(Client_Socket,Request + Bytes_received,Max_Reads - Bytes_received,0);
    if (R_received  == EAGAIN || R_received  == EWOULDBLOCK){
        perror("recv");
        exit(EXIT_FAILURE);
    }
    if (R_received < 0){
        perror("recv");
        exit(EXIT_FAILURE);
    }
    std::cout << R_received << std::endl;
    Bytes_received += R_received;
    Request[Bytes_received] = 0;
    std::string get_when_ended(Request);
    // std::cout << get_when_ended << std::endl;
    // int l;
    if (get_when_ended.find("\r\n\r\n") != std::string::npos){
        // std::string k = get_when_ended.substr(l,get_when_ended.size());
        // std::cout << k << std::endl;
        // std::cout << "\rhellowrold" << std::endl;
        // std::cout << "-----------SERACH---------------" << std::endl;
    }
}