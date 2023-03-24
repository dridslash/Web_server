/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client_Smár.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 16:59:52 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/24 17:13:43 by mnaqqad          ###   ########.fr       */
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
        
    private:
        std::pair<int,int>Client_Ip_Port_Connected;
        std::string temp_req;
};