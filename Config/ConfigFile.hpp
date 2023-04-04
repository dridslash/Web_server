#pragma once

#include "../header.hpp"

class LocationBlocks {
    public:
    LocationBlocks();
    //~LocationBlocks();
    std::string                 path;
    std::string                 root;
    std::string                 AutoIndex;
    std::vector<std::string>*   httpmethods;
    std::vector<std::string>*   index;
    std::vector<std::string>*   try_files;
    std::vector<std::string>*   Return;
    std::map<std::string, std::string>*   CGI;
    void setAutoIndex(std::string);
    void setRoot(std::string);
    void setCGI(std::pair<std::string, std::string>);
    void setMethods(std::vector<std::string>);
    void setIndex(std::vector<std::string>);
    void setTryFiles(std::vector<std::string>);
    void setReturn(std::vector<std::string>);
};

class ServerBlocks {
    public:
    ServerBlocks();
    std::vector<LocationBlocks*>            Locations;
    std::string                             ServerName;
    std::string                             root;
    std::vector<std::string>                listen;
    std::vector<std::string>                index;
    std::map<std::set<int>, std::string>    ErrorPage;
    void setIndex(std::vector<std::string>);
    void setErrorPage(std::pair<std::set<int>, std::string>);
    void setServerName(std::string);
    void setRoot(std::string);
};

class Config {
    public:
        typedef std::pair<std::pair<std::string, std::string>, int> ErrorBox;
        Config();
        std::set<std::string>       Ports;
        std::string                 MaxBodySize;
        std::vector<ServerBlocks>   Servers;
        std::set<std::string>       Directives;
        int ConfigParse(char *);
        ErrorBox ServerBlock(ServerBlocks & , std::vector<std::string> & , size_t & );
        ErrorBox LocationBlock(LocationBlocks *, ServerBlocks &, std::vector<std::string> & , size_t & );
        ErrorBox DirectivesMoreThanOneValue(LocationBlocks * , ServerBlocks & , std::vector<std::string> & , size_t & i, int );
        ErrorBox DirectivesOneValue(LocationBlocks * , ServerBlocks & , std::vector<std::string> & , size_t & , int );
        void HandleErrors(ServerBlocks & , ErrorBox );
};