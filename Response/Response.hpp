#pragma once
#include "../header.hpp"

class Config;
class Server_Master;
class Derya_Request;
class Client_Gymir;
class Response {
    private:
        std::string HTTPMethod;
        std::string Path;
        std::string HTTPVersion;
        std::string QueryString;
        std::ofstream Body;
        std::map<std::string,std::string> RequestHeader;
        int StatusCode;
        std::string Host;
        std::string Port;
        std::pair<int, int>* LocationIndex;
        std::string Header;
        std::ofstream File;
        std::string StatusLine;
    public:
        Response();
        ~Response();
        std::string getHTTPMethod() const;
        std::string getHTTPVersion() const;
        std::string getQueryString() const;
        std::string getPath() const;
        int getStatusCode() const;
        void setPath(std::string);
        void setHost(std::string);
        void setPort(std::string);
        void setStatusCode(int);
        int getResponsePath(Client_Gymir* &, Server_Master&, Derya_Request&);
        std::pair<int, int> getLocationBlockOfTheRequest(Config);
        int getResourcePath(Config);
        int GetMethod(Client_Gymir* &, Server_Master&, std::string);
        int PostMethod(Client_Gymir* &, Server_Master&, std::string);
        int DeleteMethod(Client_Gymir* &, Server_Master&, std::string);
        int autoindex(const char *);
        void HandleErrorPages(Config );
        int getResourceType();
        int IsURIHasSlashAtTheEnd(std::string OldPath);
        int IsDirHaveIndexFiles(Config );
        int IfLocationHaveCGI(Client_Gymir* Client, Server_Master& Server);
        void ResponseFile(std::string &, Config, Derya_Request&);
        int RemoveDirectory(std::string);
        std::string getDesc();
        int CheckRequestLine(Config config, Derya_Request& request);
        void MakeResponse(Client_Gymir* & Client, Server_Master& Server, Derya_Request& requestFile);
        void SendResponse(Client_Gymir* & Client, Server_Master& Server);
        int ParseBody(char* buffer, Server_Master& Server);
        int InitCGI(Client_Gymir* Client, Config config);
        void InitResponseHeaders(Client_Gymir* & Client, Server_Master& Server);
        bool look_for_BWS(std::string field_name);
        int ParseCGIHeaders(char* buffer);
};