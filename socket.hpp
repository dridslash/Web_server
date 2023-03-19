#pragma once

#include "header.hpp"

class Socket {
    public:
        int SocketFd;
        struct sockaddr_in host_addr;
        int host_addrlen;
        Socket();
        ~Socket();
        int accept_new_connections(int PORT);
};