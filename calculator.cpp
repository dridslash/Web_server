#include <iostream>
#include <cstdlib>

std::pair<std::pair<int, int>, char> encoded(char* query) {
    std::string str(query);
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
    char* queryString = getenv("QUERY_STRING");
    int res;

    std::cout << "<html><head><title>Calculator App</title></head><body>";

    if (queryString == NULL) {
        std::cout << "<p>Error: No message body found.</p>";
    } else {
        std::pair<std::pair<int, int>, char> pr = encoded(queryString);
        switch (pr.second)
        {
            case '*':
                res = pr.first.first * pr.first.second;
                break;
            case '/':
                res = pr.first.first / pr.first.second;
                break;
            case '+':
                res = pr.first.first + pr.first.second;
                break;
            case '-':
                res = pr.first.first - pr.first.second;
                break;
            default:
                std::cout << "Bad Information Form" << std::endl;
                std::cout << "</body></html>";
                return 0;
        }
        printf("%d %c %d => %d", pr.first.first, pr.second, pr.first.second, res);
    }

    std::cout << "</body></html>";

    return 0;
}
