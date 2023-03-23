/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_Eyjafjörður.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 16:54:59 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/23 18:11:22 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server_Eyjafjörður.hpp"

int Server_Eyjafjörður::reuse = 1;

Server_Eyjafjörður::Server_Eyjafjörður(){}


Server_Eyjafjörður::Server_Eyjafjörður(int sk,const char *port):Server_Socket(sk),PORT(port){
}

int Server_Eyjafjörður::Set_up_Server(const char *port){
    
    // struct addrinfo hints , *servinfo;
    PORT = port;

    memset(&hints, 0, sizeof(hints));
    
    // hints.ai_family = AF_UNSPEC;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // const char *port = "8080";
    int get_add_errno = 0;
    if ((get_add_errno = getaddrinfo(NULL, port, &hints, &servinfo)) != 0){
        std::cerr << (get_add_errno) << std::endl;
    }
    // char hold_ip[INET_ADDRSTRLEN];
    // inet_ntop(server_struct->sin_family,&(server_struct->sin_addr),hold_ip,sizeof(hold_ip));
    Server_Socket = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);
    if (setsockopt(Server_Socket,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse)) < 0){
        perror("Setsockopt Error");
        exit(EXIT_FAILURE);
    }
    if (bind(Server_Socket,(struct sockaddr*)servinfo->ai_addr,servinfo->ai_addrlen) < 0){
        perror("Bind Error");
        exit(EXIT_FAILURE);
    }
    std::cout << "IP Bound" << std::endl;
    if (listen(Server_Socket,SOMAXCONN) < 0){
        perror("Listen Error");
        exit(EXIT_FAILURE);
    }
    std::cout << "Server up and listening..." << std::endl;

    return (0);
}