#pragma once
#include "header.hpp"

class Config;
class Request;
class Response {
    private:
        std::string HTTPMethod; // from requestParse
        std::string Path; // from requestParse
        std::string HTTPVersion; // from requestParse
        std::string Body;
        int StatusCode;
        std::string Host;
        std::string Port;
        std::pair<int, int>* LocationIndex;
        std::map<std::string, std::string> ContentTypes;
    public:
        Response();
        ~Response();
        std::map<std::string, std::string> RequestHeader;
        std::string getHTTPMethod() const;
        std::string getHTTPVersion() const;
        std::string getPath() const;
        int getStatusCode() const;
        void setPath(std::string);
        void setHost(std::string);
        void setPort(std::string);
        void setStatusCode(int);
        void StoreCharURI(std::set<char> & );
        int getResponsePath(Config, Request&);
        std::pair<int, int> getLocationBlockOfTheRequest(Config);
        int getResourcePath(Config);
        int GetMethod(Config);
        int PostMethod(Config);
        int DeleteMethod(Config);
        int autoindex(const char *);
        int HandleErrorPages(Config );
        int getResourceType();
        int IsURIHasSlashAtTheEnd();
        int IsDirHaveIndexFiles(Config );
        int IfLocationHaveCGI();
        void ResponseFile(char **, std::string &, Config, Request);
        std::string get_content_type(const char*);
};