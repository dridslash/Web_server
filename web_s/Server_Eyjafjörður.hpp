/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Eyjafjörður.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 16:54:59 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/23 13:51:10 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <deque>
#include "header.hpp"

class Server_Eyjafjörður{
    public:
        struct addrinfo hints , *servinfo;
        Server_Eyjafjörður();
        Server_Eyjafjörður(int sk);
        int Set_up_Server();
    private:
        int Server_Socket;
        // std::deuque<>
        // int Status_Code;
        
};