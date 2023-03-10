#include "RequestPart.hpp"

RequestMap::RequestMap() {}
RequestMap::~RequestMap() {}

std::string RequestMap::getHTTPMethod() const {
    return HTTPMethod;
}

std::string RequestMap::getPATH() const {
    return PATH;
}

std::string RequestMap::getHTTPVersion() const {
    return HTTPVersion;
}

void RequestMap::RequestParse(std::string s) {
    std::string copy(s);
    std::string Key;
    std::string Value;
    std::string delimiter1 = " ";
    std::string delimiter2 = "\n";
    // -------------Request Line------------------
    HTTPMethod = copy.substr(0, copy.find(delimiter1));
    copy = copy.substr(copy.find(delimiter1) + 1);
    PATH = copy.substr(0, copy.find(delimiter1));
    copy = copy.substr(copy.find(delimiter1) + 1);
    HTTPVersion = copy.substr(0, copy.find(delimiter2));
    copy = copy.substr(copy.find(delimiter2) + 1);
    // -------------HTTP Headers------------------
    while (copy.length()) {
        Key = copy.substr(0, copy.find(delimiter1) - 1);
        copy = copy.substr(copy.find(delimiter1) + 1);
        Value = copy.substr(0, copy.find(delimiter2));
        copy = copy.substr(copy.find(delimiter2) + 1);
        Request.insert(std::make_pair(Key, Value));
    }
    for (std::map<std::string, std::string>::iterator it = Request.begin(); it != Request.end(); it++)
        std::cout << it->first << " " << it->second <<std::endl;
    std::cout << Request.size() <<std::endl;
}

bool RequestMap::is_req_well_formed() const {
    return true;
}