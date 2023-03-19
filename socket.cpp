#include "socket.hpp"

#define PORT 7070

Socket::Socket() {}
Socket::~Socket() {}

int Socket::accept_new_connections(int PORT) {
    int reuse_addr = 1;  /* Used so we can re-bind to our port
    while a previous connection is still in TIME_WAIT state. */
    host_addrlen = sizeof(host_addr);
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    SocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketFd == -1) {
        perror("webserver (socket)");
        return 1;
    }
    std::cout << "socket created successfully" << std::endl;

    setsockopt(SocketFd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));

    if (bind(SocketFd, (struct sockaddr *)&host_addr, host_addrlen) != 0) {
        perror("webserver (bind)");
        return 1;
    }
    std::cout << "socket successfully bound to address" << std::endl;

    if (listen(SocketFd, SOMAXCONN) != 0) {
        perror("webserver (listen)");
        return 1;
    }
    std::cout << "server listening for connections" << std::endl;
    return 0;
}
