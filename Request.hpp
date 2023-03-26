#pragma once
#include "header.hpp"

class Config;
class Request {
    private:
        std::string HTTPMethod;
        std::string Path;
        std::string HTTPVersion;
        std::string QueryString;
        std::ofstream Body;
    public:
        Request();
        ~Request();
        std::map<std::string, std::string> RequestMap;
        std::string getHTTPMethod() const;
        std::string getPath() const;
        std::string getQueryString() const;
        std::ofstream& getBody();
        std::string getHTTPVersion() const;
        std::map<std::string, std::string> getRequestHeader() const;
        void RequestParse(std::string);
};