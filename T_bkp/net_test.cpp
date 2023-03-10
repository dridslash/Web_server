/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   net_test.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/02 16:53:37 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/08 12:17:28 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT 80
#define backlog 20
#define BUFFER_SIZE 1024

int main(){
    char buffer[BUFFER_SIZE];

    int reuse = 1;
    
    //response message
    std::string response ("HTTP/1.1 200 OK\r\n"
                        "Server: webserver-c\r\n"
                        "Content-type: text/html\r\n\r\n"
                        "<html>im the server here is you response</html>\r\n");

    //////----connection and binding ------------////
    
   int socket_file = socket(AF_INET,SOCK_STREAM,0);
   if (setsockopt(socket_file,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse)) < 0){
        perror("setsockopt failed");
        return (0);
   }
    if (socket_file == -1){
            perror("Error in creating a socket\n");
            return (0);
    }
    std::cout<<"Socket created !!\n";
    struct sockaddr_in host_addd;
    int host_addlen = sizeof(host_addd);

    host_addd.sin_family = AF_INET;
    host_addd.sin_port = htons(PORT);
    host_addd.sin_addr.s_addr = htonl(INADDR_ANY);

    struct sockaddr_in client_addd;
    int client_addlen = sizeof(client_addd);
    if (bind(socket_file, (struct sockaddr *)&host_addd, host_addlen) != 0){
        perror("Bind error");
        return (0);
    }
    std::cout << "Socket bound to address\n";

    if (listen(socket_file, backlog) != 0){
        perror("Im deaf");
        return (0);
    }

    std::cout << "Server listening\n";
    
    fd_set mother_listner;

    FD_ZERO(&mother_listner);
    FD_SET(socket_file,&mother_listner);
    int f_max = socket_file;
    
    for(;;){
        fd_set copy_ct = mother_listner;
       int rdc = select(f_max + 1, &copy_ct,nullptr,nullptr,nullptr);

        for(int i = 0; i <= f_max + 1; i++){
                int sock = i;
            if (FD_ISSET(sock,&copy_ct)){
             if (sock == socket_file){
                // ACCPET CONNECTION
                int client_socket = accept(socket_file,(struct sockaddr *)&host_addd,(socklen_t*)&host_addlen);
                //std::cout << "Enter\n";
                
                if (client_socket < 0){
                    std::cout << "Error in accepting socket\n";
                    //FD_CLR(client_socket,&mother_listner);
                    close(client_socket); 
                }
                fcntl(socket_file,F_SETFL,O_NONBLOCK);
                std::cout << "Connetion made"<<std::endl;
                FD_SET(client_socket,&mother_listner);
                if (client_socket >= f_max)
                        f_max = client_socket;
                std::string wel ("HTTP/1.1 200 OK\r\n\r\n"
                "<header>Welcome to the server</header>\r\n");
                send(client_socket,wel.c_str(),wel.size() + 1,0);
            }
            else{
                //fcntl(socket_file,F_SETFL,O_NONBLOCK);
                if (recv(sock,buffer,BUFFER_SIZE,0) <= 0){
                    close(sock);
                    FD_CLR(sock,&mother_listner);
                }
                //std::cout << "here after recv\n";
            }
            }
        }
        //}
        //else
           // continue;
    }
    
//     for(;;){
//         int new_socket  = accept(socket_file,(struct sockaddr *)&host_addd,(socklen_t*)&host_addlen);
//         if (new_socket < 0){
//             std::cout << "Error in accepting socket\n";
//             continue;
//         }
        
//         std::cout << "Connetion made\n";

//         int sock_client = getsockname(new_socket, (struct sockaddr *)&client_addd, (socklen_t*)&client_addlen);

//         if (sock_client < 0){
//             perror("Getsockname error");
//             continue;
//         }

        
//         //receive from client
//         int get_read = recv(new_socket,buffer,BUFFER_SIZE,0);
//         if (get_read < 0){
//             perror("Error in reading\n");
//             continue;
//         }
        
//         //printf("[%s:%u]\n", inet_ntoa(client_addd.sin_addr),
//               // ntohs(client_addd.sin_port));
               
//         // send a response to client
//         int send_msg = send(new_socket,response.c_str(),response.size(),0);
//         if (send_msg < 0){
//             perror("Error in sending\n");
//             continue;
//         }
//         close(new_socket);
//    }
}