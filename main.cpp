#include "socket.hpp"
#include <sstream>

#define BUFFER_SIZE 1024
#define MAX_CONNECTIONS 10

void ResponseFile(std::string & resp, char* resp1) {
    std::string newresp;

    std::ifstream myfile1("resp1");
    if ( myfile1.is_open() ) while ( myfile1 ) {
        std::getline(myfile1, newresp);
        resp.append(newresp);
        resp.append(1, '\n');
    }
    resp.append(1, '\n');
    std::ifstream myfile(resp1);
    if ( myfile.is_open() ) while ( myfile ) {
        std::getline(myfile, newresp);
        resp.append(newresp);
        resp.append(1, '\n');
    }
}
fd_set master;        /* Socket file descriptors we want to wake up for, using select() */
int maxfd; /* Maximum file descriptor number */
fd_set copy;
Socket SOCKET;
std::string resp;


void setnonblocking(int & sock)
{
	if (fcntl(sock,F_SETFL, O_NONBLOCK) < 0) {
		perror("fcntl(F_SETFL)");
		exit(EXIT_FAILURE);
	}
	return;
}

int main() {
	char buffer[BUFFER_SIZE];
    ResponseFile(resp, "index.html");
	struct sockaddr_in server_address; /* bind info structure */
	int readsocks;	     /* Number of sockets ready for reading */
    if (SOCKET.accept_new_connections()) return -1;

	// setnonblocking(SOCKET.SocketFd);
	FD_ZERO(&master);
	FD_SET(SOCKET.SocketFd, &master);
	maxfd = SOCKET.SocketFd;
    while (1) {
		copy = master;
		int socketCount = select(maxfd+1, &copy, NULL, NULL, NULL);

		for (int i = 0; i <= maxfd; i++) {
			if (FD_ISSET(i, &copy)) {
				if (i == SOCKET.SocketFd) {
					std::cout << "New client" << std::endl;
					// accept a new connection
					int connection = accept(i, NULL, NULL);
					if (connection == -1)
						perror("webserv (accept)");
					setnonblocking(connection);
					// Add the new connection to the list of connected clients
					FD_SET(connection, &master);
					if (connection > maxfd)    // keep track of the max
                        maxfd = connection;
					// Send a welcome message to the connected client
					send(connection, resp.c_str(), resp.size() + 1, 0);
				}
				else {
					std::cout << "New Message from old client" << std::endl;
					// accept a new message
					int bytesIn = recv(i, buffer, BUFFER_SIZE, 0);
					if (bytesIn <= 0) {
						// Drop the client
						close(i);
						FD_CLR(i, &master);
					}
					else {
						// Send message to other cients, and definiately NOT the listening socket
						for (int j = 0; j <= maxfd; j++) {
							if (FD_ISSET(j, &master)) {
								if (j != SOCKET.SocketFd && j != i) {
									std::ostringstream ss;
									ss << "SOCKET #" << i << "\r\n";
									std::string strOut = ss.str();
									setnonblocking(j);
									send(j, strOut.c_str(), strOut.size() + 1, 0);
								}
							}
						}
					}
				}
			}
		}
    }
    return 0;
}