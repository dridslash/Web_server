/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gymir.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 16:54:59 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/23 18:13:02 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <deque>
#include "header.hpp"
#include "ConfigFile.hpp"
#include "Server_Eyjafjörður.hpp"

class Gymir{
    public:
        Gymir();
        Gymir(const std::vector<Server_Eyjafjörður>& fill_servers);
        int Get_Ports();
        int Upping_Eyjafjörðurs(char *Config_file);
        void Apply_Servers(int number_of_ports,const char *port);
    private:
        std::vector<Server_Eyjafjörður>Servers;
        
        
};