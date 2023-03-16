/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/11 10:21:09 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/11 11:53:15 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int socket):ServerScoket(socket){}

void Server::ready_up_connection(Server & serv){
    // serv.Server_Address.sin_family = AF_INET;
    // serv.Server_Address.sin_addr;
}
