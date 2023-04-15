#include "../Client_Gymir.hpp"
#include "../Server_Master.hpp"

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
    std::map<int, std::pair<ServerBlocks, std::string> > ServersThatMatchThePort;
    std::map<std::string, int> LocationsThatMatchTheURI;
    // Storing the servers that matching the port
    for (size_t i = 0; i < config.Servers.size(); i++) {
        for (std::map<std::string, std::string>::iterator it = config.Servers[i].listen.begin(); it != config.Servers[i].listen.end(); it++) {
            if (it->first == Port)
                ServersThatMatchThePort.insert(std::make_pair(int(i), std::make_pair(config.Servers[i], it->second)));
        }
    }
    // get the Server that most matching the request by Host
    std::map<int, std::pair<ServerBlocks, std::string> >::iterator it = ServersThatMatchThePort.begin();
    for (; it != ServersThatMatchThePort.end();) {
        if ((it->second.second == "localhost" || it->second.second == "127.0.0.1") && Host == "127.0.0.1") {
            ServerThatMatchIndex = it->first;
            break;
        }
        if (it->second.second == Host || ServersThatMatchThePort.size() == 1) {
            ServerThatMatchIndex = it->first;
            break;
        }
        if (it->second.second != "") {
            std::map<int, std::pair<ServerBlocks, std::string> >::iterator ite = it;
            it++;
            ServersThatMatchThePort.erase(ite);
        }
        else it++;
    }
    if (ServerThatMatchIndex == -1) {
        // get the Server that most matching the request by ServerName
        for (it = ServersThatMatchThePort.begin(); it != ServersThatMatchThePort.end(); it++) {
            std::string HostIP;
            if (RequestHeader.find("Host") != RequestHeader.end()) {
                if (RequestHeader.at("Host").find(":") != std::string::npos)
                    HostIP = RequestHeader.at("Host").substr(0, RequestHeader.at("Host").find(":"));
                else
                    HostIP = RequestHeader.at("Host");
            }
            else HostIP = Host;
            if (RequestHeader.find("Host") != RequestHeader.end() && it->second.first.ServerName == "localhost" && HostIP == "127.0.0.1") {
                ServerThatMatchIndex = it->first;
                break;
            }
            if (it->second.first.ServerName == HostIP || ServersThatMatchThePort.size() == 1) {
                ServerThatMatchIndex = it->first;
                break;
            }
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

int Response::getResponsePath(Client_Gymir* & Client, Server_Master& Server, Derya_Request& request) {
    std::map<std::string, std::string>::iterator it = RequestHeader.find("Transfer-Encoding");
    if (it == RequestHeader.end() && RequestHeader.find("Content-Length") == RequestHeader.end() && HTTPMethod == "POST") // Bad Request (client Error, POST method need to come with Transfer-Encoding or Content-Length)
        return 400;
    std::ifstream in_file(Client->FilePath.c_str());
    in_file.seekg(0, std::ios::end);
    int file_size = in_file.tellg();
    if (file_size > atoi(Server.conf.MaxBodySize.c_str()))// Request Body too large
        return 413;
    in_file.close();
    delete LocationIndex;
    LocationIndex = new std::pair<int, int>(getLocationBlockOfTheRequest(Server.conf));
    if (LocationIndex->first == -1) // Location not found (Client Error)
        return 404;
    if (Server.conf.Servers[LocationIndex->first].Locations[LocationIndex->second]->Return->size()) { // There is redirection
        Path = Server.conf.Servers[LocationIndex->first].Locations[LocationIndex->second]->Return->at(1);
        return 301;
    }
    if (std::find(Server.conf.Servers[LocationIndex->first].Locations[LocationIndex->second]->httpmethods->begin(),
        Server.conf.Servers[LocationIndex->first].Locations[LocationIndex->second]->httpmethods->end(),
            HTTPMethod) == Server.conf.Servers[LocationIndex->first].Locations[LocationIndex->second]->httpmethods->end()) // Method not allowed
        return 405;
    int Method = (HTTPMethod == "GET") * 0 + (HTTPMethod == "POST") * 1 + (HTTPMethod == "DELETE") * 2;
    int (Response::*arr[3]) ( Client_Gymir* &, Server_Master&, std::string ) = {&Response::GetMethod, &Response::PostMethod, &Response::DeleteMethod};
    StatusCode = (this->*arr[Method])(Client, Server, request.Path);
    return StatusCode;
}

void Response::HandleErrorPages(Config config) {
    if (LocationIndex->first == -1)
        LocationIndex->first = 0;
    for (std::map<std::set<int>, std::string>::iterator it = config.Servers[LocationIndex->first].ErrorPage.begin(); it != config.Servers[LocationIndex->first].ErrorPage.end(); it++)
    {
        for (std::set<int>::iterator ite = it->first.begin(); ite != it->first.end(); ite++) {
            if (*ite == StatusCode) {
                Path = config.Servers[LocationIndex->first].root;
                Path.append(it->second);
                return ;
            }
        }
    }
}

int Response::getResourcePath(Config config) {
    struct stat sb;
    const char* RootDir;
    if (Path.find("?") != std::string::npos) {
        QueryString = Path.substr(Path.find("?") + 1);
        Path = Path.substr(0, Path.find("?"));
    }
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
    if (Path[Path.length() - 1] != '/') {
        Path = OldPath;
        Path.append("/");
        if (HTTPMethod == "DELETE")
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
    if (config.Servers[LocationIndex->first].Locations[LocationIndex->second]->index->size() == 0) {
        for (std::vector<std::string>::iterator it = config.Servers[LocationIndex->first].index.begin(); it != config.Servers[LocationIndex->first].index.end(); it++) {
            std::string PathFile = Path + *it;
            if (stat(PathFile.c_str(), &sb) == 0) {
                Path.append(*it);
                return 200;
            }
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

void Response::MakeResponse(Client_Gymir* & Client, Server_Master& Server, Derya_Request& requestFile) {
    if (StatusCode == 200) {
        StatusCode = getResponsePath(Client, Server, requestFile);
        if (StatusCode == -1) return;
        if (StatusCode == 200 && access(Path.c_str(), R_OK) == -1)
            StatusCode = 403;
        if (StatusCode != 200 && StatusCode != 301)
            HandleErrorPages(Server.conf);
    }
    if (HTTPMethod == "POST")
        remove(Client->FilePath.c_str());
    Client->binaryFile.open(Path.c_str(), std::ios::binary);
    Client->binaryFile.seekg(0, std::ios_base::end);
    Client->FileLength = Client->binaryFile.tellg();
    Client->binaryFile.seekg(0, std::ios_base::beg);
}

void Response::InitResponseHeaders(Client_Gymir* & Client, Server_Master& Server) {
    if (Client->IsCGI) {
        std::map<std::string, std::string>::iterator it = RequestHeader.find("Location");
        if (it != RequestHeader.end()) {
            Path = it->second;
            RequestHeader.clear();
            RequestHeader.insert(std::make_pair("Location", Path));
            StatusCode = 301;
        }
        else if (RequestHeader.find("Status") != RequestHeader.end()) {
            if (RequestHeader.at("Status").size() != strspn(RequestHeader.at("Status").c_str(), "0123456789"))
                StatusCode = 400;
            else {
                StatusCode = atoi(RequestHeader.at("Status").c_str());
                RequestHeader.erase(it);
            }
        }
        if (StatusCode != 200 && StatusCode != 301)
            HandleErrorPages(Server.conf);
        if (StatusCode != 301 && RequestHeader.find("Content-Length") == RequestHeader.end())
            RequestHeader.insert(std::make_pair("Content-Length", std::to_string(Client->FileLength)));
        StatusLine.clear();
        StatusLine.append(HTTPVersion + " " + std::to_string(StatusCode) + " ");
        StatusLine.append(getDesc());
        return ;
    }
    StatusLine.clear();
    StatusLine.append(HTTPVersion + " " + std::to_string(StatusCode) + " ");
    StatusLine.append(getDesc());
    RequestHeader.clear();
    if (StatusCode == 301)
        RequestHeader.insert(std::make_pair("Location", Path));
    else {
        RequestHeader.insert(std::make_pair("Content-Length", std::to_string(Client->FileLength)));
        RequestHeader.insert(std::make_pair("Content-Type", Server.getContentType(Path.c_str())));
    }
}

void Response::SendResponse(Client_Gymir* & Client, Server_Master& Server) {
    int S_sended;
    int ReadReturn;
    if (!Client->IsHeaderSended) {
        InitResponseHeaders(Client, Server);
        std::string newresp = StatusLine;
        for (std::map<std::string, std::string>::iterator it = RequestHeader.begin(); it != RequestHeader.end(); it++)
            newresp.append("\r\n" + it->first + ": " + it->second);
        newresp.append("\r\n\r\n");
        std::strcpy(Client->temp_resp, newresp.c_str());
        ReadReturn = newresp.size();
    }
    else {
        Client->binaryFile.read(Client->temp_resp, Max_Writes);
        ReadReturn = Client->binaryFile.gcount();
    }
    S_sended = send(Client->Client_Socket, Client->temp_resp, ReadReturn, 0);
    if (Client->IsHeaderSended)
        Client->Bytes_Sended += S_sended;
    if (S_sended <= 0 || (Client->IsHeaderSended && ReadReturn < Max_Writes)) {
        Server.PrintStatus(Client->Client_Socket, 0, Path, StatusCode);
        Client->binaryFile.close();
        Client->Client_Hamr = Response_Completed;
    }
    Client->IsHeaderSended = true;
}

std::string Response::getDesc() {
    switch (StatusCode) {
        case 200:
            return "OK";
        case 201:
            return "Created";
        case 204:
            return "No Content";
        case 301:
            return "Moved Permanently";
        case 400:
            return "Bad Request";
        case 403:
            return "Forbidden";
        case 404:
            return "Not Found";
        case 405:
            return "Method Not Allowed";
        case 409:
            return "Conflict";
        case 413:
            return "Content Too Large";
        case 414:
            return "URI Too Long";
        case 500:
            return "Internal Server Error";
        case 501:
            return "Not Implemented";
    }
    return "Not Implemented";
}