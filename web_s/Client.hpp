#pragma once

#include "header.hpp"

class ClientInfo;

class ClientsBox {
    public:
        std::set<ClientInfo> ConnectedClients;
};

class ClientInfo {
	public:
		explicit ClientInfo(int socket):ClSocket(socket){}
		// socklen_t client_address_len;
		// struct sockaddr_in ClientAddress;
		int ClSocket;
		std::string ClientRequest;
		int BytesReceived;
};