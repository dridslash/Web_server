#include "Request.hpp"
#include "ConfigFile.hpp"

#define NEWLINE "\n"

Request::Request() {}
Request::~Request() {}

std::string Request::getHTTPMethod() const {
    return HTTPMethod;
}

std::string Request::getPath() const {
    return Path;
}

std::string Request::getHTTPVersion() const {
    return HTTPVersion;
}

void Request::RequestParse(std::string s) {
    std::string Key, Value, Line;
    // -------------Request Line------------------
    Line = s.substr(0, s.find(NEWLINE));
    s = s.substr(s.find(NEWLINE) + 1);
    std::stringstream ss(Line);
    ss >> HTTPMethod >> Path >> HTTPVersion;
    // -------------HTTP Headers------------------
    Line = s.substr(0, s.find(NEWLINE));
    while (Line.length() > 1) {
        std::stringstream ss(Line);
        ss >> Key >> Value;
        RequestMap.insert(std::make_pair(Key.substr(0, Key.length() - 1), Value));
        s = s.substr(s.find(NEWLINE) + 1);
        Line = s.substr(0, s.find(NEWLINE));
    }
    if (s.length()) Body = s;
}