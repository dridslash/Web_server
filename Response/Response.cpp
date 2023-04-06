#include "Response.hpp"
#include "../Derya_Request.hpp"
#include "../Config/ConfigFile.hpp"
#include "../Client_Smár.hpp"
#include "../Server_Eyjafjörður.hpp"

Response::Response() : StatusCode(200) { LocationIndex = new std::pair<int, int>(-1, -1); }
Response::~Response() { delete LocationIndex; }
std::string Response::getHTTPMethod() const { return HTTPMethod; }
std::string Response::getPath() const { return Path; }
std::string Response::getHTTPVersion() const { return HTTPVersion; }
int Response::getStatusCode() const { return StatusCode; }
void Response::setHost(std::string name) { Host = name; }
void Response::setPort(std::string name) { Port = name; }
void Response::setPath(std::string name) { Path = name; }
void Response::setStatusCode(int code) { StatusCode = code; }

std::pair<int, int>  Response::getLocationBlockOfTheRequest(Config config) {
    std::string LocationPath;
    int ServerThatMatchIndex = -1;
    std::map<int, ServerBlocks> ServersThatMatchThePort;
    std::map<std::string, int> LocationsThatMatchTheURI;
    // Storing the servers that matching the port
    for (size_t i = 0; i < config.Servers.size(); i++) {
        for (size_t j = 0; j < config.Servers[i].listen.size(); j++) {
            if (config.Servers[i].listen[j] == Port)
                ServersThatMatchThePort.insert(std::make_pair(int(i), config.Servers[i]));
        }
    }
    // get the Server that most matching the request
    for (std::map<int, ServerBlocks>::iterator it = ServersThatMatchThePort.begin(); it != ServersThatMatchThePort.end(); it++) {
        if (it->second.ServerName == "localhost" && Host == "127.0.0.1") {
            ServerThatMatchIndex = it->first;
            break;
        }
        if (it->second.ServerName == Host || ServersThatMatchThePort.size() == 1) {
            ServerThatMatchIndex = it->first;
            break;
        }
    }
    // if there is no one match in server_name, then the server is the first one of "ServersThatMatchThePort"
    if (ServerThatMatchIndex == -1 && ServersThatMatchThePort.size() > 1)
        ServerThatMatchIndex = ServersThatMatchThePort.begin()->first;
    // get the Location that matching the request URI
    if (ServerThatMatchIndex >= 0) {
        for (size_t i = 0; i < config.Servers[ServerThatMatchIndex].Locations.size(); i++) {
            // get the path of location that included in the request URI
            LocationPath = config.Servers[ServerThatMatchIndex].Locations[i]->path;
            if (Path == LocationPath) {
                LocationsThatMatchTheURI.clear();
                LocationsThatMatchTheURI.insert(std::make_pair(LocationPath, (int)i));
                break;
            }
            if (Path.find(LocationPath) != std::string::npos)
                LocationsThatMatchTheURI.insert(std::make_pair(LocationPath, i));

        }
        if (LocationsThatMatchTheURI.size() > 0) {
            std::map<std::string, int>::iterator it = --LocationsThatMatchTheURI.end();
            return std::make_pair(ServerThatMatchIndex, it->second);
        }
    }
    return std::make_pair(-1, -1);
}

int Response::getResponsePath(Config config, Derya_Request& request) {
    std::map<std::string, std::string>::iterator it = RequestHeader.find("Transfer-Encoding");
    if (it != RequestHeader.end() && it->second == "chunked")//Not Implemented (Server Error)
        return 501;
    if (it == RequestHeader.end() && RequestHeader.find("Content-Length") == RequestHeader.end() && HTTPMethod == "POST")// Bad Request (client Error, POST method need to come with Transfer-Encoding or Content-Length)
        return 400;
    // std::ifstream in_file("Body.txt");
    // in_file.seekg(0, std::ios::end);
    // int file_size = in_file.tellg();
    // if (file_size > stoi(config.MaxBodySize))// Request Body too large
    //     return 413;
    delete LocationIndex;
    LocationIndex = new std::pair<int, int>(getLocationBlockOfTheRequest(config));
    if (LocationIndex->first == -1) // Location not found (Client Error)
        return 404;
    if (config.Servers[LocationIndex->first].Locations[LocationIndex->second]->Return->size()) { // There is redirection
        Path = config.Servers[LocationIndex->first].Locations[LocationIndex->second]->Return->at(1);
        return 301;
    }
    if (std::find(config.Servers[LocationIndex->first].Locations[LocationIndex->second]->httpmethods->begin(),
        config.Servers[LocationIndex->first].Locations[LocationIndex->second]->httpmethods->end(),
            HTTPMethod) == config.Servers[LocationIndex->first].Locations[LocationIndex->second]->httpmethods->end()) // Method not allowed
        return 405;
    int Method = (HTTPMethod == "GET") * 0 + (HTTPMethod == "POST") * 1 + (HTTPMethod == "DELETE") * 2;
    int (Response::*arr[3]) ( Config, std::string ) = {&Response::GetMethod, &Response::PostMethod, &Response::DeleteMethod};
    StatusCode = (this->*arr[Method])(config, request.Path);
    return StatusCode;
}

int Response::HandleErrorPages(Config config) {
    if (LocationIndex->first == -1)
        LocationIndex->first = 0;
    for (std::map<std::set<int>, std::string>::iterator it = config.Servers[LocationIndex->first].ErrorPage.begin(); it != config.Servers[LocationIndex->first].ErrorPage.end(); it++)
    {
        for (std::set<int>::iterator ite = it->first.begin(); ite != it->first.end(); ite++) {
            if (*ite == StatusCode) {
                Path = config.Servers[LocationIndex->first].root;
                Path.append(it->second);
                return 1;
            }
        }
    }
    return 200;
}

int Response::getResourcePath(Config config) {
    struct stat sb;
    const char* RootDir;
    if (config.Servers[LocationIndex->first].Locations[LocationIndex->second]->root.size()) RootDir = config.Servers[LocationIndex->first].Locations[LocationIndex->second]->root.c_str();
    else RootDir = config.Servers[LocationIndex->first].root.c_str();
	if (stat(RootDir, &sb)) return 404;
    Path.erase(0, config.Servers[LocationIndex->first].Locations[LocationIndex->second]->path.size());
    Path.insert(0, "/");
    Path.insert(0, RootDir);
	if (stat(Path.c_str(), &sb)) return 404;
    return 200;
}

int Response::getResourceType() {
    struct stat sb;
	stat(Path.c_str(), &sb);
    if ( S_ISDIR(sb.st_mode) ) // it's a directory
        return 1;
    return 0;
}

int Response::IsURIHasSlashAtTheEnd(std::string OldPath) {
    if (Path.back() != '/') {
        Path = OldPath;
        Path.append("/");
        if (HTTPMethod == "POST")
            return 409;
        return 301;
    }
    return 200;
}

int Response::IsDirHaveIndexFiles(Config config) {
    struct stat sb;
    for (std::vector<std::string>::iterator it = config.Servers[LocationIndex->first].Locations[LocationIndex->second]->index->begin(); it != config.Servers[LocationIndex->first].Locations[LocationIndex->second]->index->end(); it++) {
        std::string PathFile = Path + *it;
        if (stat(PathFile.c_str(), &sb) == 0) {
            Path.append(*it);
            return 200;
        }
    }
    for (std::vector<std::string>::iterator it = config.Servers[LocationIndex->first].index.begin(); it != config.Servers[LocationIndex->first].index.end(); it++) {
        std::string PathFile = Path + *it;
        if (stat(PathFile.c_str(), &sb) == 0) {
            Path.append(*it);
            return 200;
        }
    }
    return 403;
}

int Response::CheckRequestLine(Config config, Derya_Request& request) {
    Path = request.Path;
    HTTPMethod = request.HTTPMethod;
    HTTPVersion = request.HTTPVersion;
    RequestHeader = request.RequestHeader;
    std::string CharURI = "ABCDEFGHIJKLMNOPQRSTUVWXZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
    for (std::string::size_type i = 0; i < Path.size(); i++) { // if request uri contain a character not allowed
        if (CharURI.find(Path[i]) == std::string::npos) {
            StatusCode = 400;
            break ;
        }
    }
    if (StatusCode == 200 && Path.size() > 2048) // if URI contain more than 2048 chars
        StatusCode = 414;
    int Method = (HTTPMethod == "GET") * 1 + (HTTPMethod == "POST") * 2 + (HTTPMethod == "DELETE") * 3;
    if (StatusCode == 200 && Method == 0)
        StatusCode = 400;
    if (StatusCode != 200)
        HandleErrorPages(config);
    return StatusCode;
}

void Response::MakeResponse(Client_Smár* & Client, Config config, Derya_Request& requestFile) {
    if (StatusCode == 200) {
        StatusCode = getResponsePath(config, requestFile);
        if (StatusCode == -1) return;
        if (StatusCode >= 400)
            HandleErrorPages(config);
    }
    Client->binaryFile.open(Path.c_str(), std::ios::binary);
    Client->binaryFile.seekg(0, std::ios_base::end);
    Client->FileLength = Client->binaryFile.tellg();
    Client->binaryFile.seekg(0, std::ios_base::beg);
}

void Response::SendResponse(Client_Smár* & Client, Server_Eyjafjörður& Server) {
    int S_sended;
    int ReadReturn;
    if (!Client->IsHeaderSended) {
        std::string newresp = HTTPVersion;
        newresp.append(" " + std::to_string(StatusCode) + " ");
        newresp.append(getDesc());
        if (StatusCode == 301) 
            newresp.append("Location: " + Path);
        else if (Header.size()) {
            newresp.append(Header);
            newresp.append("\r\nContent-Length: ");
            newresp.append(std::to_string(Client->FileLength));
        } else {
            newresp.append("Content-Type: ");
            newresp.append(Server.getContentType(Path.c_str()));
            newresp.append("\r\nContent-Length: ");
            newresp.append(std::to_string(Client->FileLength));
        }
        newresp.append("\r\n\r\n");
        std::strcpy(Client->temp_resp, newresp.c_str());
        ReadReturn = newresp.size();
    }
    else {
        Client->binaryFile.read(Client->temp_resp, Max_Writes);
        ReadReturn = Client->binaryFile.gcount();
    }
    S_sended = send(Client->Client_Socket, Client->temp_resp, ReadReturn, 0);
    if (S_sended < 0 || (Client->IsHeaderSended && ReadReturn < Max_Writes)) {
        Server.PrintStatus(Client->Client_Socket, 0, Path, StatusCode);
        Client->binaryFile.close();
        Client->Client_Hamr = Response_Completed;
    }
    Client->IsHeaderSended = true;
    return ;
}

std::string Response::getDesc() {
    switch (StatusCode) {
        case 200:
            return "OK\r\n";
        case 201:
            return "Created\r\n";
        case 204:
            return "No Content\r\n";
        case 301:
            return "Moved Permanently\r\n";
        case 400:
            return "Bad Request\r\n";
        case 403:
            return "Forbidden\r\n";
        case 404:
            return "Not Found\r\n";
        case 405:
            return "Method Not Allowed\r\n";
        case 409:
            return "Conflict\r\n";
        case 413:
            return "Content Too Large\r\n";
        case 414:
            return "URI Too Long\r\n";
        case 500:
            return "Internal Server Error\r\n";
        case 501:
            return "Not Implemented\r\n";
    }
    return "Not Implemented\r\n";
}