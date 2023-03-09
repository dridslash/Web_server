#include "socket.hpp"
#include <sstream>
#include <netdb.h>
#include <sys/event.h>
#include <sys/time.h>
#include <map>

#define BUFFER_SIZE 1024

std::map<std::string, std::string> ContentTypes;

void ResponseFile(std::string & resp, const char* resp1) {
	std::map<std::string, std::string>::iterator formula = ContentTypes.find(strrchr(resp1, '.'));
    std::string newresp;

    std::ifstream myfile1("resp1");
    if ( myfile1.is_open() ) {
    	std::getline(myfile1, newresp);
		while ( myfile1 ) {
			resp.append(newresp);
			resp.append(1, '\n');
			std::getline(myfile1, newresp);
    	}
	}
	resp.append("Content-Type: ");
	if (formula != ContentTypes.end()) resp.append(formula->second);
	else resp.append("application/octet-stream");
    resp.append(2, '\n\n');
    std::ifstream myfile(resp1);
    if ( myfile.is_open() ) {
    	std::getline(myfile, newresp);
		while ( myfile ) {
			resp.append(newresp);
			resp.append(1, '\n');
			std::getline(myfile, newresp);
    	}
	}
}


void setnonblocking(int & sock)
{
	if (fcntl(sock,F_SETFL, O_NONBLOCK) < 0) {
		perror("fcntl(F_SETFL)");
		exit(1);
	}
	return;
}

void PleinContentType(std::map<std::string, std::string> & map) {
	map[".css"] = "text/css";
	map[".csv"] = "text/csv";
	map[".gif"] = "image/gif";
	map[".htm"] = "text/html";
	map[".html"] = "text/html";
	map[".ico"] = "image/x-icon";
	map[".jpeg"] = "image/jpeg";
	map[".jpg"] = "image/jpeg";
	map[".js"] = "application/javascript";
	map[".json"] = "application/javascript";
	map[".pdf"] = "application/pdf";
	map[".png"] = "image/png";
	map[".svg"] = "image/svg+xml";
	map[".txt"] = "text/plain";
	map[".jpg"] = "image/jpeg";
}

int main() {
	PleinContentType(ContentTypes);
	fd_set master;
	int maxfd;
	fd_set copy;
	Socket SOCKET;
	std::string resp;
	char buffer[BUFFER_SIZE];
    ResponseFile(resp, "index.html");
    if (SOCKET.accept_new_connections()) return -1;

	FD_ZERO(&master);
	FD_ZERO(&copy);
	FD_SET(SOCKET.SocketFd, &master);
	maxfd = SOCKET.SocketFd;
    while (1) {
		copy = master;
		int socketCount = select(maxfd+1, &copy, NULL, NULL, NULL);

		for (int i = 0; i <= maxfd; i++) {
			if (FD_ISSET(i, &copy)) {
				if (i == SOCKET.SocketFd) {
					int connection = accept(i, NULL, NULL);
					if (connection == -1)
						perror("webserv (accept)");
					FD_SET(connection, &master);
					if (connection > maxfd)    // keep track of the max
                        maxfd = connection;
					setnonblocking(connection);
					int bytesIn = recv(connection, buffer, BUFFER_SIZE, 0);
					if (bytesIn <= 0) {
						close(connection);
						FD_CLR(connection, &master);
					}
					else {
						std::cout << "New client" << std::endl;
						std::cout << buffer << std::endl;
						send(connection, resp.c_str(), resp.size() + 1, 0);
					}
				}
				else {
					// accept a new message
					setnonblocking(i);
					int bytesIn = recv(i, buffer, BUFFER_SIZE, 0);
					if (bytesIn <= 0) {
						close(i);
						FD_CLR(i, &master);
					}
					else {
						std::cout << "New Message from old client" << std::endl;
						std::ostringstream ss;
						ss << "SOCKET #" << i << "\r\n";
						std::string strOut = ss.str();
						send(i, strOut.c_str(), strOut.size() + 1, 0);
					}
				}
			}
		}
    }
    return 0;
}