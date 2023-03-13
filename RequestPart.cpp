#include "RequestPart.hpp"

#define NEWLINE "\n"

RequestMap::RequestMap() : StatusCode(0) {}
RequestMap::~RequestMap() {}

std::string RequestMap::getHTTPMethod() const {
    return HTTPMethod;
}

std::string RequestMap::getPath() const {
    return Path;
}

std::string RequestMap::getHTTPVersion() const {
    return HTTPVersion;
}

int RequestMap::getStatusCode() const {
    return StatusCode;
}

void RequestMap::RequestParse(std::string s) {
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
        Request.insert(std::make_pair(Key.substr(0, Key.length() - 1), Value));
        s = s.substr(s.find(NEWLINE) + 1);
        Line = s.substr(0, s.find(NEWLINE));
    }
    if (s.length()) Body = s;
}

void RequestMap::StoreCharURI(std::set<char> & CharURI) {
    char mychars[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Z', 'a', 'b', 'c', 'e', 'f', 'g', 'h',
    'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w' ,'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5' ,'6', '7', '8', '9', '-', '.', '_', '~', ':', '/', '?',
    '#', '[', ']', '@', '!', '$', '&', 39, '(', ')', '*', '+', ',', ';', '=', '%'};
    CharURI.insert(mychars, mychars + 83);
}

bool RequestMap::IsReqWillFormed() {
    // store all characters allowed in the URI
    std::set<char> CharURI;
    StoreCharURI(CharURI);
    std::map<std::string, std::string>::iterator it = Request.find("Transfer-Encoding");
    if (it != Request.end() && it->second == "chunked") {
        StatusCode = 501;
        return false;
    }
    if (it == Request.end() && Request.find("Content-Length") == Request.end() && HTTPMethod == "POST") {
        StatusCode = 400;
        return false;
    }
    for (std::string::size_type i = 0; i < Path.size(); i++) {
        if (CharURI.find(Path[i]) == CharURI.end()) {
            StatusCode = 400;
            return false;
        }
    }
    return true;
}