#include <iostream>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <string>

#define PORT 8080
#define BUFFER_SIZE 1024

void ResponseFile(std::string & resp) {
    std::string newresp;

    std::ifstream myfile1("resp1");
    if ( myfile1.is_open() ) while ( myfile1 ) {
        std::getline(myfile1, newresp);
        resp.append(newresp);
        resp.append(1, '\n');
    }
    resp.append(1, '\n');
    std::ifstream myfile("index.html");
    if ( myfile.is_open() ) while ( myfile ) {
        std::getline(myfile, newresp);
        resp.append(newresp);
        resp.append(1, '\n');
    }
}

int CreateSocket(int & SocketFd) {
    SocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketFd == -1) {
        perror("webserver (socket)");
        return 1;
    }
    std::cout << "socket created successfully" << std::endl;
    return 0;
}

int BindSocket(int SocketFd, struct sockaddr_in & host_addr, int host_addrlen) {
    if (bind(SocketFd, (struct sockaddr *)&host_addr, host_addrlen) != 0) {
        perror("webserver (bind)");
        return 1;
    }
    std::cout << "socket successfully bound to address" << std::endl;
    return 0;
}

int ListenSocket(int & SocketFd) {
    if (listen(SocketFd, SOMAXCONN) != 0) {
        perror("webserver (listen)");
        return 1;
    }
    std::cout << "server listening for connections" << std::endl;
    return 0;
}


void FillServerAddress(struct sockaddr_in & host_addr) {
    int host_addrlen = sizeof(host_addr);
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
}

int main() {
    int SocketFd;
    close(3);
    char buffer[BUFFER_SIZE];
    std::string resp;
    // Create a Response File
    ResponseFile(resp);

    // Create a socket
    if (CreateSocket(SocketFd)) return 1;

    // Create the address to bind the socket to
    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);
    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);
    FillServerAddress(host_addr);

    // Bind the socket to the address
    if (BindSocket(SocketFd, host_addr, host_addrlen)) return 1;

    // Listen for incoming connections
    if (ListenSocket(SocketFd)) return 1;

    while (1) {

        int newSocket = accept(SocketFd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);
        if (newSocket < 0) {
            perror("webserver (accept)");
            continue;
        }


        int sockn = getsockname(newSocket, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen);
        if (sockn != 0) {
            perror("webserver (getsockname)");
            continue;
        }
        char method[BUFFER_SIZE], path[BUFFER_SIZE], version[BUFFER_SIZE];
        sscanf(buffer, "%s %s %s", method, path, version);
        // std::cout << inet_ntoa(client_addr.sin_addr) << " " << ntohs(client_addrlen) << " "  << method << " " << path << version << std::endl;

        std::cout << "connection accepted" << std::endl;
        int valRead = recv(newSocket, buffer, BUFFER_SIZE, 0);
        // printf("%.*s",valRead, buffer);
        // std::cout << buffer << std::endl;
        if (valRead == -1) {
            perror("webserver (read)");
            continue;
        }
        // sending response
        int valwrite = send(newSocket, resp.c_str(), resp.size(), 0);
        if (valwrite < 0) {
            perror("webserver (write)");
            continue;
        }
        close(newSocket);
    }
    return 0;
}