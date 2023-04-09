
#pragma once

#include "header.hpp"
#include "Derya_Request.hpp"
#include "Response/Response.hpp"
#define Max_Reads 6000
#define Max_Writes 1024

// class Derya_Request;

enum Hávamál {
    Still_Reading_Request,
    Request_Completed,
    Response_Still_Serving,
    Response_Completed
};

class Client_Smár{
    public:
        Hávamál Client_Hamr;
        Response ResponsePath;
        Derya_Request Request_parser;
        Client_Smár();
        Client_Smár(int sockcl);
        ~Client_Smár();
        int Client_Socket;
        struct sockaddr_in Own_addr;
        static Client_Smár* Draupnir_Smár(int sockcl);
        void Set_up_ip_port();
        std::pair<char*, uint16_t >Client_Ip_Port_Connected;
        // char Request[Max_Reads];
        std::string Request;
        char temp_resp[Max_Writes];
        int Bytes_received;
        int Bytes_Sended;
        bool IsHeaderSended;
        std::ifstream binaryFile;
        int FileLength;
};

