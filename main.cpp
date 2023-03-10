#include "RequestPart.hpp"
#include "socket.hpp"

class client_info {
	public:
		client_info(int sock) : socket(sock) {
			address_length = sizeof(addr);
		};
		socklen_t address_length;
		struct sockaddr_in addr;
		int socket;
		std::string request;
		int BytesReceived;
};

std::map<std::string, std::string> ContentTypes;

std::string get_content_type(const char* resp) {
	std::string Default = "application/octet-stream";
	std::map<std::string, std::string>::iterator formula = ContentTypes.find(strrchr(resp, '.'));
	if (formula != ContentTypes.end())
		return formula->second;
	return Default;
}

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
	resp.append(get_content_type(resp1));
    resp.append(1, '\n');
    std::ifstream myfile(resp1);
    if ( myfile.is_open() ) {
    	resp.append(1, '\n');
    	std::getline(myfile, newresp);
		while ( myfile ) {
			resp.append(newresp);
			resp.append(1, '\n');
			std::getline(myfile, newresp);
    	}
	}
}

std::list<client_info>::iterator get_client(int sock, std::list<client_info> & li) {
	for (std::list<client_info>::iterator it = li.begin(); it != li.end(); it++)
		if (it->socket == sock) return it;
	li.push_back(client_info(sock));
	return --li.end();
}

void drop_client(int sock, std::list<client_info> & li) {
	for (std::list<client_info>::iterator it = li.begin(); it != li.end(); it++) {
		if (it->socket == sock)
			li.erase(it);
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

void PleinContentTypes(std::map<std::string, std::string> & map) {
	std::ifstream myfile("mime.types");
	std::string key, value;
    if ( myfile.is_open() ) {
		while ( myfile ) {
			myfile >> value >> key;
			map.insert(std::make_pair(key, value));
		}
	}
}

int main() {
	RequestMap requestFile;
	PleinContentTypes(ContentTypes);
	// fd_set master;
	std::list<client_info> master;
	int maxfd;
	fd_set copy;
	Socket SOCKET;
	std::string resp;
	char buffer[BUFFER_SIZE];
    ResponseFile(resp, "index.html");
    if (SOCKET.accept_new_connections()) return -1;

	FD_ZERO(&copy);
	// FD_SET(SOCKET.SocketFd, &master);
	get_client(SOCKET.SocketFd, master);
	maxfd = SOCKET.SocketFd;
    while (1) {
		for(std::list<client_info>::iterator it = master.begin(); it != master.end(); it++)
			FD_SET(it->socket, &copy);
		// copy = master;
		int socketCount = select(maxfd+1, &copy, NULL, NULL, NULL);

		for (int i = 0; i <= maxfd; i++) {
			if (FD_ISSET(i, &copy)) {
				if (i == SOCKET.SocketFd) {
					int connection = accept(i, NULL, NULL);
					if (connection == -1)
						perror("webserv (accept)");
					get_client(connection, master);
					if (connection > maxfd)
                        maxfd = connection;
					setnonblocking(connection);
					int bytesIn = recv(connection, buffer, BUFFER_SIZE, 0);
					if (bytesIn <= 0) {
						drop_client(connection, master);
						close(connection);
					}
					else {
						std::string str(buffer);
						requestFile.RequestParse(str);
						std::cout << "New client" << std::endl;
						send(connection, resp.c_str(), resp.size() + 1, 0);
					}
				}
				else {
					std::cout << "Message from old client" << std::endl;
					setnonblocking(i);
					int bytesIn = recv(i, buffer, BUFFER_SIZE, 0);
					if (bytesIn <= 0) {
						drop_client(i, master);
						close(i);
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