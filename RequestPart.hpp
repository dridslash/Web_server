#include "header.hpp"

class RequestMap {
    private:
        std::string HTTPMethod;
        std::string PATH;
        std::string HTTPVersion;
    public:
        std::map<std::string, std::string> Request;
        std::string getHTTPMethod() const;
        std::string getPATH() const;
        std::string getHTTPVersion() const;
        void RequestParse(std::string RequestFile);
};