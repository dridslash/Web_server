#include "RequestPart.hpp"

std::string RequestMap::getHTTPMethod() const {
    return HTTPMethod;
}

std::string RequestMap::getPATH() const {
    return PATH;
}

std::string RequestMap::getHTTPVersion() const {
    return HTTPVersion;
}

void RequestMap::RequestParse(std::string RequestFile) {
    RequestFile >> HTTPMethod >> PATH >> HTTPVersion;
    std::cout << HTTPMethod << " " << PATH << " " <<  HTTPVersion << std::endl;
}