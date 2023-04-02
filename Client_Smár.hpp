/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client_Smár.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 16:59:52 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/04/02 15:41:42 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "header.hpp"
#define Max_Reads 2047
#define Max_Writes 60000

enum Hávamál{
    Still_Reading_Request,
    Request_Completed,
    Response_Still_Serving,
    Response_Completed
};

class Client_Smár{
    public:
        Hávamál Client_Hamr;
        Client_Smár();
        Client_Smár(int sockcl);
        ~Client_Smár();
        int Client_Socket;
        struct sockaddr_in Own_addr;
        static Client_Smár* Draupnir_Smár(int sockcl);
        void Set_up_ip_port();
        std::pair<char*, uint16_t >Client_Ip_Port_Connected;
        char Request[Max_Reads + 1];
        char temp_resp[Max_Writes];
        int Bytes_received;
        int Bytes_Sended;
        bool IsHeaderSended;
        // ================= String Part ===================
        std::string resp;
        // ================= String Part ===================
};