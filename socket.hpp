#pragma once

#include <iostream>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <fcntl.h>

class Socket {
    public:
        int SocketFd;
        struct sockaddr_in host_addr;
        int host_addrlen;
        Socket();
        ~Socket();
        int accept_new_connections();
};