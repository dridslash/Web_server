#pragma once
#include "../header.hpp"

class Config;
class Server_Eyjafjörður;
class Derya_Request;
class Client_Smár;
class Response {
    private:
        std::string HTTPMethod;
        std::string Path;
        std::string HTTPVersion;
        std::string QueryString;
        std::ofstream Body;
        int StatusCode;
        std::string Host;
        std::string Port;
        std::pair<int, int>* LocationIndex;
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
        int getResponsePath(Config, Derya_Request&);
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
        void ResponseFile(std::string &, Config, Derya_Request&);
        int RemoveDirectory(std::string);
        std::string getDesc();
        int CheckRequestLine(Config config, Derya_Request& request);
        void MakeResponse(Client_Smár* & Client, Config config, Derya_Request& requestFile);
        void SendResponse(Client_Smár* & Client, Server_Eyjafjörður& Server);

};