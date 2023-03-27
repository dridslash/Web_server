/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gymir.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 16:54:59 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/26 16:24:00 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <deque>
#include "header.hpp"
#include "ConfigFile.hpp"
#include "Server_Eyjafjörður.hpp"
#include "Client_Smár.hpp"

class Gymir{
    public:
        Gymir();
        Gymir(const std::vector<Server_Eyjafjörður*>& fill_servers);
        ~Gymir();
        int Upping_Eyjafjörðurs(char *Config_file);
        void Create_Servers_Acc_Port(int number_of_ports,const char *port);
        Server_Eyjafjörður* Search_in_Servers(int fd,std::vector<Server_Eyjafjörður*>& Serv_All);
        int multiplexing(std::vector<Server_Eyjafjörður*>& Serv_All);
    private:
         
        static Config conf; 
};