/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client_Smár.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 16:59:52 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/25 12:19:49 by mnaqqad          ###   ########.fr       */
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

Client_Smár::Client_Smár(int sockcl):Client_Socket(sockcl){
    
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