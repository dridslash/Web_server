#pragma once
#include "header.hpp"

class Config;
class Request {
    private:
        std::string HTTPMethod;
        std::string Path;
        std::string HTTPVersion;
        std::string Body;
    public:
        Request();
        ~Request();
        std::map<std::string, std::string> RequestMap;
        std::string getHTTPMethod() const;
        std::string getPath() const;
        std::string getHTTPVersion() const;
        void RequestParse(std::string);
};