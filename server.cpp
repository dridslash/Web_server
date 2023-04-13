/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/09 17:22:51 by sennaama          #+#    #+#             */
/*   Updated: 2023/04/13 09:11:40 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <iostream>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <sys/types.h>
// #include <unistd.h>
// #include <fcntl.h>
// #define PORT 8080
// // server::server():clients()
// // {
// // 	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
// //         ft_exit("socket");
// // 	addrlen = sizeof(address);
// // 	address.sin_family = AF_INET;
// //     address.sin_addr.s_addr = INADDR_ANY;
// //     address.sin_port = htons( PORT );
// // 	memset(address.sin_zero, '\0', sizeof address.sin_zero);
// // }

// // server::server(const server& obj)
// // {
// // 	*this = obj;
// // }

// // server& server::operator=(const server& obj)
// // {
// // 	if (this != &obj)
// // 	{
// // 		clients = obj.clients;
// // 		addrlen = obj.addrlen;
// // 		server_socket = obj.server_socket;
// // 		address.sin_family = obj.address.sin_family;
// //     	address.sin_addr.s_addr = obj.address.sin_addr.s_addr;
// //     	address.sin_port = obj.address.sin_addr.s_addr;
// // 		memcpy(&address.sin_zero, &obj.address.sin_zero, sizeof(obj.address.sin_zero));
// // 	}
// //     return (*this);
// // }

// // server::~server(){}

// // void server::process(const char *response)
// // {
// // 	int     opt = true;
// // 	int		valrecv;
// // 	fd_set  readfds; 
// // 	int		max_sd;
// // 	int		sd;
	
// // 	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )  
// //         ft_exit("setsockopt");  
// //     if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
// //         ft_exit("bind");
// //     if (listen(server_socket, 3) < 0)
// //         ft_exit("listen");
// //     std::cout<<"\n+++++++ Waiting for connections ++++++++\n\n";
// //     while(1)
// //     {
// // 		FD_ZERO(&readfds);
// // 		FD_SET(server_socket, &readfds);
// // 		max_sd = server_socket;

// //         for (std::vector<Client *>::iterator it= clients.begin(); it != clients.end(); ++it)
// //         {
// //             Client* client = *it;
// //             sd = client->getClientSocket();
// //             if (sd > 0)
// //                 FD_SET(sd, &readfds);
// //             if (sd > max_sd)
// //                 max_sd = sd;
// //         }
// // 		int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
// //         if ((activity < 0))
// //             ft_exit("select");
// // 		if (FD_ISSET(server_socket, &readfds))
// //         {
// //             if ((sd = accept(server_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
// //                 ft_exit("server:accept");
// //             std::cout<<"New connection: "<< inet_ntoa(address.sin_addr)<<" "<<ntohs(address.sin_port)<<std::endl;
// //             char buffer[30000] = {0};
// //             if (recv(sd, buffer, sizeof(buffer), 0) > 0) {
// //                 std::cout << "Request:\n" << buffer << "\n";
// //                 Client* client = new Client("", buffer, sd);
// //                     clients.push_back(client);
// //                 } else {
// //                     close(sd);
// //                 }
// //         }
// //         for (std::vector<Client *>::iterator it= clients.begin(); it != clients.end(); ++it)
// //         {
// //             Client* client = *it;
// //             sd = client->getClientSocket();
// //             if (FD_ISSET(sd, &readfds)) {
// //                 char buffer[30000] = {0};
// //                 valrecv = recv(sd, buffer, sizeof(buffer), 0);
// //                 if (valrecv <= 0) {
// //                     std::cout<<"Connection closed: "<< inet_ntoa(address.sin_addr)<< ntohs(address.sin_port)<<std::endl;
// //                     close(sd);
// //                     clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
// //                     delete client;
// //                 } else {
// //                     printf("Request: %s", buffer);
// //                     client->setRequest(buffer);
// //                     send(sd, response, strlen(response), 0);
// //                 }
// //             }
// //         }
// //     }
// // }

// void ft_exit(std::string str)
// {
// 	std::cout << str << std::endl;
// 	exit(1);
// }

// void process(const char *response)
// {
//     // fd_set	master;
//     fd_set	read_fds;
// 	fd_set	write_fds;
// 	fd_set	read_fds_copy;
// 	fd_set	write_fds_copy;
//     int		listener;
// 	int		newfd;
// 	char	buf[256];
// 	int     opt = true;
// 	int		fdmax;
// 	int		server_socket,addrlen;
// 	struct sockaddr_in	address;
	
// 	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
//         ft_exit("socket");
// 	addrlen = sizeof(address);
// 	address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons( PORT );
// 	memset(address.sin_zero, '\0', sizeof address.sin_zero);
// 	// FD_ZERO(&master);
// 	FD_ZERO(&read_fds);
// 	FD_ZERO(&write_fds);
// 	listener = server_socket;
// 	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0 )  
//         perror("setsockopt");
//     if (bind(server_socket, (struct sockaddr *)&address, addrlen) < 0)
//         perror("bind");
//     if (listen(server_socket, SOMAXCONN) < 0)
//         perror("listen");
// 	FD_SET(listener, &read_fds);
// 	fdmax = listener;
	
// 	while (true)
// 	{
// 		fd_set	read_fds_copy = read_fds;
// 		fd_set	write_fds_copy = write_fds;
// 		if (select(fdmax + 1, &read_fds, &write_fds, NULL, NULL) == -1)
// 			std::cout<<"Error: select"<<std::endl;
// 		for (int i = 0; i < fdmax + 1; i++)
// 		{
// 			if (FD_ISSET(i, &read_fds))
// 			{
// 				if (i == listener)
// 				{
// 					addrlen = sizeof(address);
//                     newfd = accept(listener, (struct sockaddr *)&address, (socklen_t*)&addrlen);
// 					if (newfd == -1)
// 						std::cout<<"Error:accept"<<std::endl;
// 					else 
// 					{
// 						FD_SET(newfd, &read_fds);
// 						FD_SET(newfd,&write_fds);
// 						if (newfd > fdmax)
// 							fdmax = newfd;
// 							// std::cout << "Connection made"<<std::endl;
// 						std::cout<<"New connection: "<< i << " " << inet_ntoa(address.sin_addr)<<" "<<ntohs(address.sin_port)<<std::endl;
// 					}
// 				}else{
// 					recv(i,buf,sizeof(buf),0);
// 					// std::cout << buf << std::endl;
// 				}
// 			}
// 			else{
// 				send(i,response,sizeof(response),0);
// 				FD_CLR(i,&read_fds);
// 				FD_CLR(i,&write_fds);
// 				close(i);
// 			}
// 		}
// 	}
    
// }

// int main(int argc, char** argv)
// {
// 	const char* response ("HTTP/1.1 200 OK\r\n"
//                         "Server: webserver-c\r\n"
//                         "Content-type: text/html\r\n\r\n"
//                         "<html>im the server here is you response</html>\r\n");
// 	process(response);
// }