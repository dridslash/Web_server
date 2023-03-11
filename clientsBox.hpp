#pragma once

#include "header.hpp"

class clientsBox {
    public:
        std::set<clientInfo> connectedClients;
        clientInfo & getClient(int sock);
        void dropClient(int sock);
        std::string get_client_address(int sock);
        fd_set wait_on_clients(int sock);
        void send_400(int sock);
        void send_404(int sock);
        void serve_resource(int sock);
};

class clientInfo {
	public:
		clientInfo(int sock);
		socklen_t address_length;
		struct sockaddr_in addr;
		int socket;
		std::string request;
		int BytesReceived;
};