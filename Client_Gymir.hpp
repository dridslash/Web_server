#pragma once
#include "Derya_Request.hpp"

#define Max_Reads 6000
#define Max_Writes 1024

enum Hávamál {
    Still_Reading_Request,
    Request_Completed,
    Response_Still_Serving,
    Response_Completed
};

class Response;
class Client_Gymir {
    public:
        Hávamál Client_Hamr;
        Response ResponsePath;
        Derya_Request Request_parser;
        Client_Gymir();
        Client_Gymir(int sockcl);
        ~Client_Gymir();
        int Client_Socket;
        struct sockaddr_in Own_addr;
        static Client_Gymir* Draupnir_Smár(int sockcl);
        void Set_up_ip_port();
        std::pair<char*, uint16_t >Client_Ip_Port_Connected;
        std::string Request;
        char temp_resp[Max_Writes];
        int Bytes_received;
        int Bytes_Sended;
        bool IsHeaderSended;
        std::ifstream binaryFile;
        int FileLength;
        int fd[2];
        char Arg0[PATH_MAX];
        char realPATH[PATH_MAX];
        char *PathInfo;
        pid_t c_pid;
        bool IsCGI;
        std::string FilePath;
};