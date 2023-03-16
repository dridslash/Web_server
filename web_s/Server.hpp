/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/11 10:21:09 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/16 12:02:49 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "header.hpp"

class Server{
    explicit Server(int socket);
    int ServerScoket;
    // struct sockaddr_in Server_Address;
    // socklen_t server_address_len;

    void ready_up_connection(Server & serv);
};