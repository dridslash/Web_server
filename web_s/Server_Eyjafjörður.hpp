/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Eyjafjörður.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 16:54:59 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/24 17:50:34 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "header.hpp"
#include "ConfigFile.hpp"
#include "Client_Smár.hpp"

class Server_Eyjafjörður{
    public:
        struct addrinfo hints , *servinfo;
        static int kq;
        const char *PORT;
        Server_Eyjafjörður();
        ~Server_Eyjafjörður();
        Server_Eyjafjörður(int sk,const char *port);
        bool Add_Event_to_queue_ker(int &fd , int filter);
        bool Delete_Event_to_queue_ker(int &fd , int filter);
        int Set_up_Server(const char *port);
        void Change_Socket_To_Non_Block(int &fd);
        int Get_Server_Socket()const;
        std::map<int,Client_Smár>Clients;
    private:
        static int reuse;
        int Server_Socket;
};