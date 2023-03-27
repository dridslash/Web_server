#include "Response.hpp"
#include "Request.hpp"
#include "ConfigFile.hpp"
#include <math.h>
#include <cstdlib>


Response::Response() : StatusCode(0) {
    std::ifstream myfile("public/mime.types");
	std::string key, value;
    if ( myfile.is_open() ) {
		while ( myfile ) {
			myfile >> value >> key;
			ContentTypes.insert(std::make_pair(key, value));
		}
	}
}
Response::~Response() {}

std::string Response::getHTTPMethod() const {
    return HTTPMethod;
}

std::string Response::getPath() const {
    return Path;
}

std::string Response::getHTTPVersion() const {
    return HTTPVersion;
}

int Response::getStatusCode() const {
    return StatusCode;
}

void Response::setHost(std::string name) { Host = name; }
void Response::setPort(std::string name) { Port = name; }
void Response::setPath(std::string name) { Path = name; }
void Response::setStatusCode(int code) { StatusCode = code; }

int Response::autoindex(const char *dirpath) {
    DIR *dir;
    struct stat result;
    struct dirent *entry;
    dir = opendir(dirpath);
    if (dir == NULL) {
        printf("Error: could not open directory\n");
        return 404;
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
    return 200;
}

int Response::GetMethod(Config config, std::string OldPath) {
    StatusCode = getResourcePath(config);
    if (StatusCode != 200) return StatusCode;
    if (getResourceType()) { // if it's directory
        StatusCode = IsURIHasSlashAtTheEnd(OldPath);
        if (StatusCode != 200) return StatusCode;
        if (config.Servers[LocationIndex->first].Locations[LocationIndex->second]->AutoIndex == "on") {
            StatusCode = autoindex(Path.c_str());
            return StatusCode;
        }
        StatusCode = IsDirHaveIndexFiles(config);
        if (StatusCode != 200) return StatusCode;
    }
    if (IfLocationHaveCGI(config) == 0) return StatusCode;
    return StatusCode;
}

int Response::PostMethod(Config config, std::string OldPath) {
    StatusCode = getResourcePath(config);
    if (StatusCode != 200) return StatusCode;
    if (getResourceType()) { // if it's directory
        StatusCode = IsURIHasSlashAtTheEnd(OldPath);
        if (StatusCode != 200) return StatusCode;
        StatusCode = IsDirHaveIndexFiles(config);
        if (StatusCode != 200) return StatusCode;
    }
    if (IfLocationHaveCGI(config) == 0) return StatusCode;
    return 201;
}

int Response::DeleteMethod(Config config, std::string OldPath) {
    StatusCode = getResourcePath(config);
    if (StatusCode != 200) return StatusCode;
    if (getResourceType()) { // if it's directory
        StatusCode = IsURIHasSlashAtTheEnd(OldPath);
        if (StatusCode != 200) return StatusCode;
        if (IfLocationHaveCGI(config) == 0) {
            StatusCode = IsDirHaveIndexFiles(config);
            //CGI JOB
            if (StatusCode != 200) return StatusCode;
        }
        StatusCode = RemoveDirectory(Path.c_str());
        if (StatusCode != 204) return StatusCode;
    }
    if (IfLocationHaveCGI(config) == 0)
        remove(Path.c_str());
    return 204;
}

int Response::RemoveDirectory(std::string path) {
    DIR *dir;
    struct stat result;
    struct dirent *entry;
    dir = opendir(path.c_str());
    if (dir == NULL) return 403;
    readdir(dir);
    readdir(dir);
    while ((entry = readdir(dir)) != NULL) {
        if (path.back() != '/') path.append("/");
        Path = path + entry->d_name;
        stat(Path.c_str(), &result);
        if ( entry->d_type == DT_DIR ) {
            if (rmdir(Path.c_str()) == -1) {
                StatusCode = RemoveDirectory(Path);
                if (StatusCode != 204) return StatusCode;
            }
        } else {
            if (remove(Path.c_str()) == -1) {
                if (access(Path.c_str(), W_OK) == -1) return 403;
                return 500;
            }
        }
    }
    Path = Path.substr(0, Path.find_last_of("/") + 1);
    if (path == Path) {
        Path = Path.substr(0, Path.find_last_of("/"));
        if (rmdir(Path.c_str()) == -1) return 500;
    }
    return 204;
}

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

int Response::getResponsePath(Config config, Request& request) {
    LocationIndex = new std::pair<int, int>(-1, -1);
    Path = request.getPath();
    HTTPMethod = request.getHTTPMethod();
    HTTPVersion = request.getHTTPVersion();
    QueryString = request.getQueryString();
    RequestHeader = request.getRequestHeader();
    std::string CharURI = "ABCDEFGHIJKLMNOPQRSTUVWXZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";

    std::map<std::string, std::string>::iterator it = RequestHeader.find("Transfer-Encoding");
    if (it != RequestHeader.end() && it->second == "chunked")//Not Implemented (Server Error)
        return 501;
    if (it == RequestHeader.end() && RequestHeader.find("Content-Length") == RequestHeader.end() && HTTPMethod == "POST")// Bad Request (client Error, POST method need to come with Transfer-Encoding or Content-Length)
        return 400;
    for (std::string::size_type i = 0; i < Path.size(); i++) {// if request uri contain a character not allowed
        if (CharURI.find(Path[i]) == std::string::npos)
            return 400;
    }
    if (Path.size() > 2048)// if URI contain more than 2048 chars
        return 414;
    std::ifstream in_file("Body.txt");
    in_file.seekg(0, std::ios::end);
    int file_size = in_file.tellg();
    if (file_size > stoi(config.MaxBodySize))// Request Body too large
        return 413;
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
    StatusCode = (this->*arr[Method])(config, request.getPath());
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

// std::cout << "-------------------------------------------------------------------------------" << std::endl;

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

int Response::IfLocationHaveCGI(Config config) {
    // char PATH_INFO[PATH_MAX];
    // std::string PathInfo;
    if (config.Servers[LocationIndex->first].Locations[LocationIndex->second]->CGI->size() == 0) return 0;
    // if (config.Servers[LocationIndex->first].Locations[LocationIndex->second]->CGI->find("php") != config.Servers[LocationIndex->first].Locations[LocationIndex->second]->CGI->end()
    //     && Path.find(".php") != std::string::npos) {
    //         PathInfo = config.Servers[LocationIndex->first].Locations[LocationIndex->second]->CGI->find("php")->second;
    // }
    // else if (config.Servers[LocationIndex->first].Locations[LocationIndex->second]->CGI->find("cgi") != config.Servers[LocationIndex->first].Locations[LocationIndex->second]->CGI->end()
    //     && Path.find(".cgi") != std::string::npos) {
    //         PathInfo = config.Servers[LocationIndex->first].Locations[LocationIndex->second]->CGI->find("cgi")->second;
    // }
    // std::cout << PathInfo << std::endl;
    // char* res = realpath(PathInfo.c_str(), PATH_INFO);
    // if (!res) {
    //     perror("realpath");
    //     exit(EXIT_FAILURE);
    // }
    // std::cout << PATH_INFO << std::endl;
    std::ofstream f("public/cgiOutput.html");
    int InputFile = open("Body.txt", O_RDONLY, 0777);
    int fd[2];
    pipe(fd);
    pid_t c_pid = fork();
    if (c_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (c_pid == 0) {
        dup2(fd[1], 1);
        dup2(InputFile, 0);
        
        // setenv("QUERY_STRING", QueryString.c_str(), 0);
        // setenv("REQUEST_METHOD", HTTPMethod.c_str(), 0);
        // char *env[] = {"PATH_INFO=/Users/oouazize/Desktop/webserv/php-cgi", NULL};
        // setenv("REDIRECT_STATUS", "200", 0);
        // setenv("PATH_INFO", "Users/oouazize/Desktop/webserv/php-cgi", 0);
        // setenv("CONTENT_LENGTH", "1024", 0);
        // setenv("PATH_INFO", PATH_INFO, 0);
        extern char** environ;
        // char *PATH = strcpy(PATH, Path.c_str());
        // char *args[] = {"/Users/oouazize/Desktop/webserv/php-cgi", "public/index.php", NULL};
        // execve(args[0], args, env);
        perror("execve");
        exit(EXIT_FAILURE);
    }
    char buffer[100000];
    int r = read(fd[0], buffer, 10000);
    buffer[r] = '\0';
    close(fd[1]);
    std::cout << buffer;
    Path = "public/cgiOutput.html";
    close(fd[0]);
    StatusCode = 200;
    return 1;
}

void Response::ResponseFile(std::string & resp, Config config, Request& requestFile) {
    std::string newresp;
    StatusCode = getResponsePath(config, requestFile);
	if (StatusCode >= 400)
		HandleErrorPages(config);
    resp = HTTPVersion;
    resp.append(" " + std::to_string(StatusCode) + " ");
    resp.append(getDesc());
    if (StatusCode == 301) {
        resp.append("Location: " + Path);
        resp.append("\r\n");
    }
    else {
        resp.append("Content-Type: ");
        resp.append(getContentType(Path.c_str()));
        resp.append("\r\n");
        std::ifstream myfile(Path.c_str());
        if ( myfile.is_open() ) {
            resp.append("\r\n");
            std::getline(myfile, newresp);
            while ( myfile ) {
                resp.append(newresp);
                resp.append("\r\n");
                std::getline(myfile, newresp);
            }
        }
    }
    // std::cout << resp << std::endl;
}

std::string Response::getContentType(const char* resp) {
	std::string Default = "application/octet-stream";
	std::map<std::string, std::string>::iterator formula = ContentTypes.find(strrchr(resp, '.'));
	if (formula != ContentTypes.end())
		return formula->second;
	return Default;
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
    return NULL;
}