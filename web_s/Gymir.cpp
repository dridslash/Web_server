/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gymir.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 16:54:59 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/23 18:26:02 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Gymir.hpp"

Gymir::Gymir(){}

Gymir::Gymir(const std::vector<Server_Eyjafjörður>& fill_servers):Servers(fill_servers){}


void Gymir::Apply_Servers(int number_of_ports, const char *port){
        Server_Eyjafjörður Serv;
        Serv.Set_up_Server(port);
        Servers.push_back(Serv);
}

int Gymir::Upping_Eyjafjörðurs(char *Config_file){
    Config conf;

    conf.ConfigParse(Config_file);
    int MAX_PORTS = conf.Ports.size();
    Servers.reserve(MAX_PORTS);
   for(std::set<std::string>::iterator it = conf.Ports.begin();it != conf.Ports.end();it++){
        Apply_Servers(MAX_PORTS, (*it).c_str());
    }
    
    // for(int i =0 ;i < MAX_PORTS ;i++){
    //     std::cout << Servers[i].PORT << std::endl;
    // }
    return (0);
}