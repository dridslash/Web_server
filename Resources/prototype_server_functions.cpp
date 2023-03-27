/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prototype_server_functions.cpp                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnaqqad <mnaqqad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/23 10:00:18 by mnaqqad           #+#    #+#             */
/*   Updated: 2023/03/23 14:44:43 by mnaqqad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


//------------------------POLL_FUNCTIONS-----------------------------

// void insert_pfd(struct pollfd *hpfds, int new_socket_fd , int &count_fds , int &size_fds){
//     if (count_fds == size_fds){
//         size_fds *= 2;
//         hpfds = static_cast<pollfd*>(realloc(hpfds, sizeof(pollfd) * size_fds));
//     }

//     hpfds[count_fds].fd = new_socket_fd;
//     hpfds[count_fds].events = POLLIN;

//     count_fds++;
    
// }

// void erase_pfd(struct pollfd *hpfds, int i){
//     hpfds[i].fd = -1;
// }

// void ADD_EVENT(int fd , int filter, int flags){
//     struct kevent Take_Eevent;
// }

//-----------------------------------------------------

 //r- ---------SELECT--------------
    
    // fd_set read_fds;
    // fd_set write_fds;

    // FD_ZERO(&read_fds);
    // FD_ZERO(&write_fds);
    // FD_SET(server_socket,&read_fds);
    // FD_SET(server_socket,&write_fds);
    // int f_max = server_socket;
    // struct timeval time;
    // time.tv_sec = 5;
    // time.tv_usec = 0;
    
    // for(;;){
    //     fd_set copy_read = read_fds;
    //     fd_set copy_write = write_fds;
    //    int rdc = select(f_max + 1, &copy_read,&copy_write,nullptr,nullptr);
    //    if (rdc < 0){
    //         perror("select error");
    //         break;
    //    }
    //    std::cout << rdc << "for the wi\n";
    //    if (rdc == 0){
    //      perror("select timeout");
    //      break;
    //    }

    //     for(int i = 0; i <= f_max; i++){
    //             int sock = i;
    //         if (FD_ISSET(sock,&copy_read)){
    //          if (sock == server_socket){
    //             // ACCPET CONNECTION
    //             int client_socket = accept(server_socket,reinterpret_cast<struct sockaddr*>(&host_addd),
    //                         reinterpret_cast<socklen_t*>(&host_addlen));
    
    //             if (client_socket < 0){
    //                 std::cout << "Error in accepting socket\n";
    //                 FD_CLR(client_socket,&read_fds);
    //                 close(client_socket); 
    //             }
    //             if (recv(client_socket,buffer,BUFFER_SIZE,0) <= 0){
    //                 close(client_socket);
    //                 FD_CLR(client_socket,&read_fds);
    //             }
    //             std::cout << "Connetion made"<<std::endl;
    //             FD_SET(client_socket,&read_fds);
    //             if (client_socket >= f_max)
    //                     f_max = client_socket;
    //             std::string wel ("HTTP/1.1 200 OK\r\n\r\n"
    //             "<header>Welcome to the server</header>\r\n");
    //             send(client_socket,wel.c_str(),wel.size() + 1,0);
    //         }
    //         else{
    //             if (recv(sock,buffer,BUFFER_SIZE,0) <= 0){
    //                 close(sock);
    //                 FD_CLR(sock,&read_fds);
    //             }
    //         }
    //         }
    //     }
    // }
    
    //r-/////////////////////////////////////////////////////


    //y- -----------------------------POLL------------------------------
    
    // int sockets_count = 0;
    
    // int poll_fd_size = 1;
    
    // struct pollfd *sockets_fds = static_cast<pollfd*>(malloc(sizeof(pollfd) * poll_fd_size));

    // sockets_fds[0].fd = server_socket;
    // sockets_fds[0].events = POLLIN;

    // sockets_count++;
    

    // while(true){
    //     int numbers_from_poll = poll(sockets_fds,sockets_count, -1);
    //     if (numbers_from_poll == -1){
    //         perror("poll error");
    //         exit(1);
    //     }
    //     for(int i = 0; i < sockets_count; i++){
    //         if (sockets_fds[i].revents & POLLIN){
    //             if (sockets_fds[i].fd == server_socket){
    //                  int client_socket = accept(server_socket,reinterpret_cast<struct sockaddr*>(&host_addd),
    //                         reinterpret_cast<socklen_t*>(&host_addlen));
    
    //             if (client_socket < 0){
    //                 std::cout << "Error in accepting socket\n";
    //                 // erase_pfd(sockets_fds,i);
    //                 close(client_socket); 
    //             }
    //             if (recv(client_socket,buffer,BUFFER_SIZE,0) <= 0){
    //                 close(client_socket);
    //                 erase_pfd(sockets_fds,client_socket);
    //             }
    //             std::cout << buffer << std::endl;
    //             std::cout << "Connetion made"<<std::endl;
    //             insert_pfd(sockets_fds,client_socket,sockets_count,poll_fd_size);
    //             std::string wel ("HTTP/1.1 200 OK\r\n\r\n"
    //             "<header>Welcome to the server</header>\r\n");
    //             send(client_socket,wel.c_str(),wel.size() + 1,0);
    //             }
    //             else{
    //                 if (recv(i,buffer,BUFFER_SIZE,0) <= 0){
    //                 close(i);
    //                 erase_pfd(sockets_fds,i);
    //                 }
    //             }
    //         }
    //     }
    // }
    
    //y- ----------------------------------------------------------

    //b- /////////////////////////////---KQUEUE---------/////////////////////
    // add_event(server_socket, EVFILT_READ | EVFILT_WRITE);
    // struct kevent evv[max_events];
    // for(;;){
    //     int n_ev = kevent(kq,NULL,0,evv,max_events,NULL);
    //     if (n_ev < 0){
    //         perror("kevent");
    //     }
    //     for(int i = 0; i < n_ev; i++){
    //         int fd = static_cast<int>(evv[i].ident);
    //             if (evv[i].filter == EVFILT_READ){
    //                 if (fd == server_socket){
    //             // ACCPET CONNECTION
    //             int client_socket = accept(server_socket,reinterpret_cast<struct sockaddr*>(&host_addd),
    //                         reinterpret_cast<socklen_t*>(&host_addlen));
    
    //             if (client_socket < 0){
    //                 std::cout << "Error in accepting socket\n";
    //                 close(client_socket); 
    //             }
    //             add_event(client_socket , EVFILT_READ);
    //             add_event(client_socket , EVFILT_WRITE);
    //             std::cout << "Connetion made"<<std::endl;
    //             std::string wel ("HTTP/1.1 200 OK\r\n\r\n"
    //             "<header>Welcome to the server</header>\r\n");
    //             send(client_socket,wel.c_str(),wel.size() + 1,0);
    //             }
    //             else {
    //                  if (recv(fd,buffer,BUFFER_SIZE,0) <= 0){
    //                     perror("recv");
    //                     close(i);
    //                 }
    //                 std::cout << buffer << std::endl;
    //             }
    //         }
    //     }
    // }
        
    

    // if (errno == EINTR){
    //     std::cout << "signal\n";
    //     return (0);
    // }
    

    //b- ////////////////////////////////////////////////////////////