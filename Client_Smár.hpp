/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client_Smár.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 16:59:52 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/27 09:16:30 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "header.hpp"
#define Max_Reads 1027

class Client_Smár{
    public:
        Client_Smár();
        Client_Smár(int sockcl);
        ~Client_Smár();
        int Client_Socket;
        struct sockaddr_in Own_addr;
        static Client_Smár* Draupnir_Smár(int sockcl);
        void Set_up_ip_port();
        std::pair<char*, uint16_t >Client_Ip_Port_Connected;
        std::string temp_req;
        std::string temp_resp;
        
    private:
};