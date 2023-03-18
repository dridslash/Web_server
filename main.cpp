#include "Response.hpp"
#include "Request.hpp"
#include "socket.hpp"
#include "ConfigFile.hpp"


void setnonblocking(int & sock)
{
	if (fcntl(sock,F_SETFL, O_NONBLOCK) < 0) {
		perror("fcntl(F_SETFL)");
		exit(1);
	}
	return;
}

int main(int arc, char **arv) {
	if (arc != 2) return -1;
	Request requestFile;
	Config config;
	if (config.ConfigParse(arv[1]) == -1)
		return  -1;
	Response ResponsePath;
	fd_set master;
	int maxfd;
	fd_set copy;
	Socket SOCKET;
	std::string resp;
	char buffer[BUFFER_SIZE];
    if (SOCKET.accept_new_connections()) return -1;
	FD_ZERO(&copy);
	FD_SET(SOCKET.SocketFd, &master);
	maxfd = SOCKET.SocketFd;
    while (1) {
		copy = master;
		select(maxfd+1, &copy, NULL, NULL, NULL);
		for (int i = 0; i <= maxfd; i++) {
			if (FD_ISSET(i, &copy)) {
				if (i == SOCKET.SocketFd) {
					int connection = accept(i, NULL, NULL);
					if (connection == -1) {
						perror("webserv (accept)");
						close(connection);
					} else {
						FD_SET(connection, &master);
						if (connection > maxfd)
							maxfd = connection;
						std::string Host = "localhost";
						std::string Port = "7070";
						ResponsePath.setHost(Host);
						ResponsePath.setPort(Port);
					}
				}
				else {
					setnonblocking(i);
					int bytesIn = recv(i, buffer, BUFFER_SIZE, 0);
					if (bytesIn <= 0) {
						FD_CLR(i, &master);
						close(i);
					}
					else {
						std::string str(buffer);
						requestFile.RequestParse(str);
						ResponsePath.ResponseFile(arv, resp, config, requestFile);
						send(i, resp.c_str(), resp.size() + 1, 0);
						std::cout << "Response Successfully sent" << std::endl;
						resp.clear();
					}
				}
			}
		}
    }
    return 0;
}