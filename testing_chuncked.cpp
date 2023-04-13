#include "header.hpp"
#define MaxRead 10

std::string request;
std::string c;
int size;
int chunked;
int flag = 0;
char buffer[MaxRead+1];
int returnRead = 0;
int still = 0;
int SizeOld = 0;
int input = open("request", O_RDONLY, 0777);
std::ofstream output("File");

int parseReq() {
    if (chunked > size) {
        output << request;
        chunked -= size;
    }
    
    // std::cout << chunked  << ":" << size << std::endl;
    // if (chunked + 5 < size) {
    //     c = request.substr(chunked + 2);
    //     c = c.substr(0, c.find("rn"));
    //     SizeOld = request.substr(0, chunked).size();
    //     request.erase(chunked, c.size() + 4);
    //     output << request;
    //     request.clear();
    //     chunked = stoi(c);
    //     if (chunked == 0) return 1;
    //     chunked += SizeOld;
    // }
    // else {
    //     output << request;
    //     chunked -= SizeOld;
    // }
    // return 0;
}

int Reading() {
    returnRead = read(input, buffer, MaxRead);
    if (returnRead <= 0)
        return 1;
    buffer[returnRead] = 0;
    std::cout << "Return Read: " << returnRead << std::endl;
    std::cout << "Buffer: " << buffer << std::endl;
    request.append(buffer);
    size = request.size();
    std::cout << "RequestNow: " << request << std::endl;
    if (request.size() == MaxRead) {
        c = request.substr(0, request.find("rn"));
        request = request.substr(c.size() + 2);
        chunked = stoi(c);
        size -= c.size() + 2;
    }
    if (parseReq() == 1) return 1;
    std::cout << "================================================================\n";
    return 0;
}

int main() {

    int r = Reading();
    while (r == 0)
        r = Reading();
    close(input);
}