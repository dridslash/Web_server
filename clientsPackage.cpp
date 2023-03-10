#include "clientsPackage.hpp"

clientInfo & clientsPackage::getClient(int sock) {
	std::set<clientInfo>::iterator it = connectedClients.find(sock);
	if (it != connectedClients.end())
		return *it;
	std::cout << "Add new client" << std::endl;
	return li.insert(clientInfo(sock))->first;
}

void clientsPackage::dropClient(int sock) {
	std::set<clientInfo>::iterator it = connectedClients.find(sock);
	if (it != connectedClients.end()) {
		std::cout << "Drop client" << std::endl;
		connectedClients.erase(it);
		return;
	}
	std::cout << "Drop_client not found!" << std::endl;
}

std::string clientsPackage::get_client_address(int sock) {
	std::string addrBuffer;
	std::set<clientInfo>::iterator it = connectedClients.find(sock);
	if (it != connectedClients.end()) {
		getnameinfo((struct sockaddr*)&it->addr,
               it->address_length, addrBuffer, 1024, 0, 0,
               NI_NUMERICHOST);
	}
	else
		std::cout << "Get_client_adress not found!" << std::endl
	return addrBuffer;
}

fd_set clientsPackage::wait_on_clients(int sock) {
	fd_set reads;
	FD_ZERO(&reads);
	FD_SET(server, &reads);
	int MaxFd = server;
	for (std::set<clientInfo>::iterator it = connectedClients.begin(); it != connectedClients.end(); it++) {
		FD_SET(it->socket, &reads);
		if (it->socket > MaxFd)
			MaxFd = it->socket;
	}
	if (select(MaxFd+1, &reads, 0, 0, 0) < 0) {
		perror("select");
		exit(1);
	}
	return reads;
}

void clientsPackage::send_400(int sock) {
	const char *c400 = "HTTP/1.1 400 Bad Request\r\n"
           "Connection: close\r\n"
           "Content-Length: 11\r\n\r\nBad Request";
    send(sock, c400, strlen(c400), 0);
	dropClient(sock);
}

void clientsPackage::send_404(int sock) {
	const char *c404 = "HTTP/1.1 404 Not Found\r\n"
           "Connection: close\r\n"
           "Content-Length: 9\r\n\r\nNot Found";
    send(sock, c404, strlen(c404), 0);
    dropClient(sock);
}

void clientsPackage::serve_resource() {

}


// ----------------clientInfo------------------------

clientInfo::clientInfo(int sock) : socket(sock) {
	address_length = sizeof(addr)
}