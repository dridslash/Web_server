#include "header.hpp"

class RequestMap {
    private:
        std::string HTTPMethod;
        std::string PATH;
        std::string HTTPVersion;
        bool IsParse;
    public:
        RequestMap();
        ~RequestMap();
        std::map<std::string, std::string> Request;
        std::string getHTTPMethod() const;
        std::string getPATH() const;
        std::string getHTTPVersion() const;
        void RequestParse(std::string s);
        bool is_req_well_formed() const;
};