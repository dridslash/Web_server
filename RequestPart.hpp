#pragma once
#include "header.hpp"

class Config;
class RequestMap {
    private:
        std::string HTTPMethod;
        std::string Path;
        std::string HTTPVersion;
        std::string Body;
        int StatusCode;
        std::string Host;
        std::string Port;
        int ServerThatMatchIndex;
        std::pair<int, int>* LocationIndex;
    public:
        RequestMap();
        ~RequestMap();
        std::map<std::string, std::string> Request;
        std::string getHTTPMethod() const;
        std::string getPath() const;
        std::string getHTTPVersion() const;
        void setHost(std::string);
        void setPort(std::string);
        void setPath(std::string);
        void setStatusCode(int);
        int getStatusCode() const;
        void RequestParse(std::string s);
        void StoreCharURI(std::set<char> & CharURI);
        int IsReqWillFormed(Config);
        std::pair<int, int> getLocationBlockOfTheRequest(Config);
        int getResourceType(Config);
        int GetMethod(Config);
        int PostMethod(Config);
        int DeleteMethod(Config);
        void autoindex(const char *dirpath);
};