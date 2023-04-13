#include "header.hpp"
#define MaxRead 45

std::string request;
std::string chunked;
int BufferSize = 0;
int ChunkedSize = -2;
int flag = 0;
char buffer[MaxRead+1];
int returnRead = 0;
int still = 0;
int SizeOld = 0;
int input = open("request", O_RDONLY, 0777);
std::ofstream output("File");

int parseReq() {

    BufferSize = request.size();
    // std::cout << "|ChunkedSize: " << ChunkedSize << std::endl;
    // std::cout << "|BufferSize: " << BufferSize << std::endl;
    // std::cout << "|Buffer: (" << request << ")" << std::endl << std::endl;
    if (ChunkedSize < 0) {
        if (ChunkedSize == -2)
            chunked = request;
        else
            chunked = request.substr(2);
        request = chunked.substr(chunked.find("rn") + 2);
        BufferSize = request.size();
        chunked = chunked.substr(0, chunked.find("rn"));
        ChunkedSize = stoi(chunked);
    }
    // std::cout << "ChunkedSize: " << ChunkedSize << std::endl;
    // std::cout << "BufferSize: " << BufferSize << std::endl;
    // std::cout << "Buffer: (" << request << ")" << std::endl;
    // std::cout << "==========================" << std::endl;
    if (ChunkedSize == 0) {
        output << request;
        request.clear();
    }
    else if (ChunkedSize < BufferSize) {
        output << request.substr(0, ChunkedSize);
        request = request.substr(ChunkedSize);
        ChunkedSize = -1;
    }
    else {
        output << request;
        ChunkedSize -= BufferSize;
        if (!ChunkedSize) ChunkedSize = -1;
        request.clear();
    }
    if (request.size() > 5 && request[2] == '0') {
        output << request.substr(5);
        request.clear();
    }
    // std::cout << "||returnRead: " << returnRead << std::endl;
    // // std::cout << "||ChunkedSize: " << ChunkedSize << std::endl;
    // std::cout << "||BufferSize: " << BufferSize << std::endl;
    // // std::cout << "||Buffer: (" << request << ")" << std::endl;
    // std::cout << "==========================" << std::endl;
    if (returnRead < MaxRead && request.size())
        parseReq();
    return 0;
}

int Reading() {
    returnRead = read(input, buffer, MaxRead);
    if (returnRead <= 0) return 1;
    buffer[returnRead] = 0;
    // std::cout << "Return Read: " << returnRead << std::endl;
    // std::cout << "Buffer: " << buffer << std::endl;
    request.append(buffer);
    // BufferSize = request.size();
    // std::cout << "RequestNow: " << request << std::endl;
    if (parseReq() == 1) return 1;
    // std::cout << "================================================================\n";
    // if (returnRead <= 0 || returnRead < MaxRead) {
    //     if (ChunkedSize < BufferSize) return 0;
    //     return 1;
    // }
    return 0;
}

int main() {

    int r = Reading();
    while (r == 0)
        r = Reading();
    close(input);
    // std::cout << "REQUEST ===> " << request << std::endl;
}