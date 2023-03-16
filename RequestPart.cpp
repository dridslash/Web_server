#include "RequestPart.hpp"
#include "ConfigFile.hpp"
#include <math.h>

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

void RequestMap::setHost(std::string name) { Host = name; }
void RequestMap::setPort(std::string name) { Port = name; }
void RequestMap::setPath(std::string name) { Path = name; }
void RequestMap::setStatusCode(int code) { StatusCode = code; }

int RequestMap::getResourceType(Config config) {
	struct stat sb;
    const char* RootDir;
    if (config.ConfigList[LocationIndex->first].LocationBlocks[LocationIndex->second]->root.size()) RootDir = config.ConfigList[LocationIndex->first].LocationBlocks[LocationIndex->second]->root.c_str();
    else RootDir = config.ConfigList[LocationIndex->first].root.c_str();
	if (stat(RootDir, &sb)) return 404;
    Path.erase(0, config.ConfigList[LocationIndex->first].LocationBlocks[LocationIndex->second]->path.size());
    Path.insert(0, "/");
    Path.insert(0, RootDir);
    std::string IndexPath;
	if (stat(Path.c_str(), &sb)) return 404;
    if ( S_ISDIR(sb.st_mode) ) { // it's a directory
        if (Path.back() != '/') {
            Path.append("/");
            if (HTTPMethod == "GET")
                return 301;
            return 409;
        }
        return 1;
    }
    return 0;
}

void RequestMap::autoindex(const char *dirpath) {
    DIR *dir;
    struct stat result;
    struct dirent *entry;
    dir = opendir(dirpath);
    if (dir == NULL) {
        printf("Error: could not open directory\n");
        exit(1);
    }
    std::ofstream outfile ("autoindex.html");
    std::string Text;
    Text = "<html>\n<head><title>Index of ";
    Text.append(dirpath);
    Text.append("</title></head>\n");
    Text.append("<body>\n<h1>Index of ");
    Text.append(dirpath);
    Text.append("</h1><hr><pre>");
    readdir(dir);
    while ((entry = readdir(dir)) != NULL) {
        Text.append("<a href=\"");
        Text.append(entry->d_name);
        Text.append("\">");
        Text.append(entry->d_name, 0, 41);
        if (strlen(entry->d_name) > 41)
            Text.append("...");
        Text.append("</a>");
        if (strlen(entry->d_name) > 41)
            Text.append(7, ' ');
        else
            Text.append(51 - strlen(entry->d_name), ' ');
        std::string PathOF = dirpath;
        PathOF.append(entry->d_name);
        if(stat(PathOF.c_str(), &result) == 0) {
            std::stringstream ss;
            std::string Size;
            time_t mod_time = result.st_ctime;
            Text.append(asctime(gmtime(&mod_time)));
            Text.erase(--Text.end());
            Text.append(14, ' ');
            if (S_ISREG(result.st_mode)) {
                off_t mod_size = result.st_size;
                ss << mod_size;
                ss >> Size;
                Text.append(Size);
            }
            else
                Text.append("-");
        }
        Text.append("\n");
    }
    Text.append("</pre><hr></body>\n</html>\n");
    outfile << Text << std::endl;
    outfile.close();
    Path = "autoindex.html";
    closedir(dir);
}

int RequestMap::GetMethod(Config config) {
    int Flag = 0;
    struct stat sb;
    int ResourceType = getResourceType(config);
	if (ResourceType == 409) return ResourceType;
    else if (ResourceType) {
        for (std::vector<std::string>::iterator it = config.ConfigList[LocationIndex->first].LocationBlocks[LocationIndex->second]->index->begin(); it != config.ConfigList[LocationIndex->first].LocationBlocks[LocationIndex->second]->index->end(); it++) {
            std::string PathFile = Path + *it;
            if (stat(PathFile.c_str(), &sb) == 0) {
                Path.append(*it);
                Flag = 1;
                break;
            }
        }
        if (!Flag) {
            for (std::vector<std::string>::iterator it = config.ConfigList[LocationIndex->first].index.begin(); it != config.ConfigList[LocationIndex->first].index.end(); it++) {
                std::string PathFile = Path + *it;
                if (stat(PathFile.c_str(), &sb) == 0) {
                    Path.append(*it);
                    Flag = 1;
                    break;
                }
            }
        }
        if (!Flag && config.ConfigList[LocationIndex->first].LocationBlocks[LocationIndex->second]->AutoIndex == "on")
            autoindex(Path.c_str());
    }
	if (ResourceType == 301) return ResourceType;
    return 200;    
}

int RequestMap::PostMethod(Config config) {

    return 201;
}

int RequestMap::DeleteMethod(Config config) {
    struct stat sb;
    int ResourceType = getResourceType(config);
    std::cout << Path << std::endl;
	if (ResourceType && ResourceType != 1) return ResourceType;
    else if (ResourceType == 1) {
        // if location doesn't have cgi()

    }
    else {
        std::cout << "cdwsx" << std::endl;
        remove(Path.c_str());
    }
    return 204;
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
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w' ,'x', 'y',
    'z', '0', '1', '2', '3', '4', '5' ,'6', '7', '8', '9', '-', '.', '_', '~', ':', '/', '?',
    '#', '[', ']', '@', '!', '$', '&', 39, '(', ')', '*', '+', ',', ';', '=', '%'};
    CharURI.insert(mychars, mychars + 84);
}

std::pair<int, int>  RequestMap::getLocationBlockOfTheRequest(Config config) {
    std::string LocationPath;
    int ServerThatMatchIndex = -1;
    std::map<int, ServerBlocks> ServersThatMatchThePort;
    std::map<std::string, int> LocationsThatMatchTheURI;
    // Storing the servers that matching the port
    for (int i = 0; i < config.ConfigList.size(); i++) {
        for (int j = 0; j < config.ConfigList[i].listen.size(); j++) {
            if (config.ConfigList[i].listen[j] == Port)
                ServersThatMatchThePort.insert(std::make_pair(i, config.ConfigList[i]));
        }
    }

    // get the Server that most matching the request
    for (std::map<int, ServerBlocks>::iterator it = ServersThatMatchThePort.begin(); it != ServersThatMatchThePort.end(); it++) {
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
        for (int i = 0; i < config.ConfigList[ServerThatMatchIndex].LocationBlocks.size(); i++) {
            // get the path of location that included in the request URI
            (config.ConfigList[ServerThatMatchIndex].LocationBlocks[i]->path.back() != '/') ? LocationPath = config.ConfigList[ServerThatMatchIndex].LocationBlocks[i]->path.append(1, '/') : LocationPath = config.ConfigList[ServerThatMatchIndex].LocationBlocks[i]->path;
            if (Path == LocationPath) {
                LocationsThatMatchTheURI.clear();
                LocationsThatMatchTheURI.insert(std::make_pair(LocationPath, i));
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

int RequestMap::IsReqWillFormed(Config config) {
    // store all characters allowed in the URI
    std::set<char> CharURI;
    StoreCharURI(CharURI);
    std::map<std::string, std::string>::iterator it = Request.find("Transfer-Encoding");
    if (it != Request.end() && it->second == "chunked")//Not Implemented (Server Error)
        return 501;
    if (it == Request.end() && Request.find("Content-Length") == Request.end() && HTTPMethod == "POST")// Bad Request (client Error, POST method need to come with Transfer-Encoding or Content-Length)
        return 400;
    for (std::string::size_type i = 0; i < Path.size(); i++) {// if request uri contain a character not allowed
        if (CharURI.find(Path[i]) == CharURI.end())
            return 400;
    }
    if (Path.size() > 2048)// if URI contain more than 2048 chars
        return 414;
    if (Body.size() > stoi(config.MaxBodySize))// Request Body too large
        return 413;
    LocationIndex = new std::pair<int, int>(getLocationBlockOfTheRequest(config));
    if (LocationIndex->first == -1) // Location not found (Client Error)
        return 404;
    if (config.ConfigList[LocationIndex->first].LocationBlocks[LocationIndex->second]->Return->size()) // There is redirection
        return 301;
    if (std::find(config.ConfigList[LocationIndex->first].LocationBlocks[LocationIndex->second]->httpmethods->begin(), config.ConfigList[LocationIndex->first].LocationBlocks[LocationIndex->second]->httpmethods->end(), HTTPMethod) == config.ConfigList[LocationIndex->first].LocationBlocks[LocationIndex->second]->httpmethods->end()) // Method not allowed
        return 405;
    int Method = (HTTPMethod == "GET") * 0 + (HTTPMethod == "POST") * 1 + (HTTPMethod == "DELETE") * 2;
    int (RequestMap::*arr[3]) ( Config ) = {&RequestMap::GetMethod, &RequestMap::PostMethod, &RequestMap::DeleteMethod};
    StatusCode = (this->*arr[Method])(config);
    return StatusCode;
}