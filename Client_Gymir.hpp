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
        int Client_Socket;
        Hávamál Client_Hamr;
        Response ResponsePath;
        Derya_Request Request_parser;
        struct sockaddr_in Own_addr;
        static Client_Gymir* Draupnir_Smár(int sockcl);
        std::pair<char*, uint16_t >Client_Ip_Port_Connected;
        int Bytes_received;
        int Bytes_Sended;
        std::string Request;
        char temp_resp[Max_Writes];
        bool IsHeaderSended;
        std::ifstream binaryFile;
        int FileLength;
        int fd[2];
        char Arg0[PATH_MAX];
        char realPATH[PATH_MAX];
        pid_t c_pid;
        bool IsCGI;
        std::string FilePath;
        std::string out;
        char RequestMethod[BUFFER_SIZE], Status[BUFFER_SIZE], HttpCookie[BUFFER_SIZE];
        char ContentLength[BUFFER_SIZE], ContentType[BUFFER_SIZE], QueryString1[BUFFER_SIZE];
        char PathInfo[BUFFER_SIZE], ScriptFileName[BUFFER_SIZE];
        int returnRead;
        std::string chunckedRequest;
        std::string chunkedBuffer;
        int ChunkedSize;
        unsigned long RequestSize;

        Client_Gymir();
        Client_Gymir(int sockcl);
        ~Client_Gymir();
        void Set_up_ip_port();
};