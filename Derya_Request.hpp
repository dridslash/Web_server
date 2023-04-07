#pragma once
#include "header.hpp"

class Derya_Request{
    public:
        Derya_Request();
        ~Derya_Request();
        std::string Request_Line;
        std::string HTTPMethod;
        std::string Path;
        std::string HTTPVersion;
        std::map<std::string,std::string> RequestHeader;

        //FUNCTION PARSING REQUEST
        int Parse_Request(std::string Request);

        //FUNCTION PARSING REQUEST LINE
        void bolkr_Request_Line(std::string request_line);

        bool look_for_BWS(std::string field_name);
};