#include "header.hpp"

std::pair<std::pair<int, int>, char> encoded(char* InputData) {
    
    std::string str(InputData);
    int n1;
    int n2;
    char Operator;
    n1 = stoi(str.substr(str.find('=') + 1, str.find('&')));
    str = str.substr(str.find('&') + 1);
    n2 = stoi(str.substr(str.find('=') + 1, str.find('&')));
    str = str.substr(str.find('&') + 1);
    std::string opr = str.substr(str.find('=') + 1, 3);
    if (opr == "%2F") Operator = '/';
    else Operator = '+';
    return std::make_pair(std::make_pair(n1, n2), Operator);
}

int main() {
     std::pair<std::pair<int, int>, char>* pr;
    char* contentLength = getenv("CONTENT_LENGTH");
    size_t len;
    sscanf(contentLength, "%ld", &len);
    char* Method = getenv("REQUEST_METHOD");
    if (strcmp(Method, "GET") == 0) {
        char * QueryString = getenv("QUERY_STRING");
        pr = new std::pair<std::pair<int, int>, char>(encoded(QueryString));
    } else {
        char InputData[len];
        read(0, InputData, len);
        pr = new std::pair<std::pair<int, int>, char>(encoded(InputData));
    }
    int res;
    std::cout << "<html><head><title>Calculator App</title></head><body>";
    switch (pr->second)
    {
        case '*':
            res = pr->first.first * pr->first.second;
            break;
        case '/':
            res = pr->first.first / pr->first.second;
            break;
        case '+':
            res = pr->first.first + pr->first.second;
            break;
        case '-':
            res = pr->first.first - pr->first.second;
            break;
        default:
            std::cout << "Bad Information Form" << std::endl;
            std::cout << "</body></html>";
            return 0;
    }
    printf("%d %c %d => %d", pr->first.first, pr->second, pr->first.second, res);

    std::cout << "</body></html>";

    delete pr;

    return 0;
}
