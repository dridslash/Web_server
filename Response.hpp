#pragma once
#include "header.hpp"

class Config;
class Request;
class Response {
    private:
        std::string HTTPMethod; // from requestParse
        std::string Path; // from requestParse
        std::string HTTPVersion; // from requestParse
        std::string QueryString; // from requestParse
        std::ofstream Body;
        int StatusCode;
        std::string Host;
        std::string Port;
        std::pair<int, int>* LocationIndex;
        std::map<std::string, std::string> ContentTypes;
        int Moved;
    public:
        Response();
        ~Response();
        std::map<std::string, std::string> RequestHeader;
        std::string getHTTPMethod() const;
        std::string getHTTPVersion() const;
        std::string getQueryString() const;
        std::string getPath() const;
        int getStatusCode() const;
        void setPath(std::string);
        void setHost(std::string);
        void setPort(std::string);
        void setStatusCode(int);
        int getResponsePath(Config, Request&);
        std::pair<int, int> getLocationBlockOfTheRequest(Config);
        int getResourcePath(Config);
        int GetMethod(Config, std::string);
        int PostMethod(Config, std::string);
        int DeleteMethod(Config, std::string);
        int autoindex(const char *);
        int HandleErrorPages(Config );
        int getResourceType();
        int IsURIHasSlashAtTheEnd(std::string OldPath);
        int IsDirHaveIndexFiles(Config );
        int IfLocationHaveCGI(Config config);
        void ResponseFile(char **, std::string &, Config, Request&);
        std::string getContentType(const char*);
        int RemoveDirectory(std::string);
        std::string getDesc();

};