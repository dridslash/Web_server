#pragma once

#include "header.hpp"

class LocationBlocks {
    public:
    LocationBlocks();
    std::string                 path;
    std::string                 root;
    std::string                 AutoIndex;
    std::vector<std::string>    httpmethods;
    std::vector<std::string>    index;
    std::vector<std::string>    try_files;
    std::vector<std::string>    Return;
    void setPath(std::string);
    void setAutoIndex(std::string);
    void setRoot(std::string);
    void setMethods(std::vector<std::string>);
    void setIndex(std::vector<std::string>);
    void setTryFiles(std::vector<std::string>);
    void setReturn(std::vector<std::string>);
};

class ServerBlocks {
    public:
    ServerBlocks();
    std::vector<LocationBlocks>   LocationBlocks;
    std::string                 ServerName;
    std::string                 root;
    std::string                 MaxBodySize;
    std::vector<std::string>    listen;
    std::vector<std::string>    index;
    std::vector<std::string>    ErrorPage;
    void setIndex(std::vector<std::string>);
    void setErrorPage(std::vector<std::string>);
    void setServerName(std::string);
    void setRoot(std::string);
    void setMaxBodySize(std::string);
};

class Config {
    public:
        typedef std::pair<std::pair<std::string, std::string>, int> ErrorBox;
        Config();
        std::vector<ServerBlocks> ConfigList;
        int ConfigParse(char *);
        std::set<std::string> Directives;
        ErrorBox ServerBlock(ServerBlocks & , std::vector<std::string> & , int & );
        ErrorBox LocationBlock(LocationBlocks &, ServerBlocks &, std::vector<std::string> & , int & );
        ErrorBox DirectivesMoreThanOneValue(LocationBlocks & , ServerBlocks & , std::vector<std::string> & , int & i, int );
        ErrorBox DirectivesOneValue(LocationBlocks & , ServerBlocks & , std::vector<std::string> & , int & , int );
        void HandleErrors(ServerBlocks & , ErrorBox );
};
