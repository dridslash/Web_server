/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   web_main.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/10 17:03:19 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/24 17:22:49 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "Client.hpp"
#include "Server_Eyjafjörður.hpp"
#include "Client_Smár.hpp"
#include "Gymir.hpp"

#define PORT 80
#define backlog 20
#define max_events 1000
#define BUFFER_SIZE 1024
// #define MICRO_PORTION 1000000


int kq = kqueue();

void add_event(int fd , int filter){
    struct kevent events[1];
    EV_SET(&events[0],fd,filter,EV_ADD | EV_CLEAR,0,0,NULL);
    kevent(kq,events,1,NULL,0,NULL);
}

int main(int ac, char **argv){

    Gymir All_servers;

    All_servers.Upping_Eyjafjörðurs(argv[1]);
    
    // //response message
    // std::string response ("HTTP/1.1 200 OK\r\n"
    //                     "Server: webserver-c\r\n"
    //                     "Content-type: text/html\r\n\r\n"
    //                     "<html>im the server here is you response</html>\r\n");

    //                     std::string msg_from_client ("HTTP/1.1 200 OK\r\n"
    //                     "Server: webserver-c\r\n"
    //                     "Content-type: text/html\r\n\r\n"
    //                     "<html>im the client here is you response</html>\r\n");

    // //////----connection and binding ------------////
    // struct addrinfo hints , *servinfo;

    // memset(&hints, 0, sizeof(hints));
    
    // // hints.ai_family = AF_UNSPEC;
    // hints.ai_family = AF_INET;
    // hints.ai_socktype = SOCK_STREAM;
    // hints.ai_flags = AI_PASSIVE;

    // const char *port = "8080";
    // int get_add_errno = 0;
    // if ((get_add_errno = getaddrinfo(NULL, port, &hints, &servinfo)) != 0){
    //     std::cerr << (get_add_errno) << std::endl;
    // }
    // char hold_ip[INET_ADDRSTRLEN];
    // // inet_ntop(server_struct->sin_family,&(server_struct->sin_addr),hold_ip,sizeof(hold_ip));
    // int server_socket = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);
    // if (setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse)) < 0){
    //     perror("Setsockopt Error");
    //     exit(EXIT_FAILURE);
    // }
    // if (bind(server_socket,(struct sockaddr*)servinfo->ai_addr,servinfo->ai_addrlen) < 0){
    //     perror("Bind Error");
    //     exit(EXIT_FAILURE);
    // }
    // std::cout << "IP Bound" << std::endl;
    // if (listen(server_socket,SOMAXCONN) < 0){
    //     perror("Listen Error");
    //     exit(EXIT_FAILURE);
    // }
    // std::cout << "Server up and listening..." << std::endl;
    // recv(server_socket,buffer,BUFFER_SIZE,0);
}