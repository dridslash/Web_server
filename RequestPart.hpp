#pragma once
#include "header.hpp"

class RequestMap {
    private:
        std::string HTTPMethod;
        std::string Path;
        std::string HTTPVersion;
        std::string Body;
        int StatusCode;
    public:
        RequestMap();
        ~RequestMap();
        std::map<std::string, std::string> Request;
        std::string getHTTPMethod() const;
        std::string getPath() const;
        std::string getHTTPVersion() const;
        int getStatusCode() const;
        void RequestParse(std::string s);
        void StoreCharURI(std::set<char> & CharURI);
        bool IsReqWillFormed();
};