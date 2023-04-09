#include "../Client_Gymir.hpp"
#include "../Server_Master.hpp"

int Response::InitCGI(Client_Gymir* Client, Config config) {
    std::string exec;
    Client->IsCGI = true;
    if (Path.find(".php") != std::string::npos) {
        exec = config.Servers[LocationIndex->first].Locations[LocationIndex->second]->CGI->at("php");
        exec.insert(0, "/");
        if (config.Servers[LocationIndex->first].Locations[LocationIndex->second]->root.size())
            exec.insert(0, config.Servers[LocationIndex->first].Locations[LocationIndex->second]->root);
        else
            exec.insert(0, config.Servers[LocationIndex->first].root);
        realpath(exec.c_str(), Client->Arg0);
    }
    else if (Path.find(".pl") != std::string::npos) {
        exec = config.Servers[LocationIndex->first].Locations[LocationIndex->second]->CGI->at("pl");
        strcpy(Client->Arg0, exec.c_str());
    }
    else
        return 0;
    
    realpath(Path.c_str(), Client->realPATH);
    std::string PATH_INFO("PATH_INFO=");
    PATH_INFO.append(Client->realPATH);
    Client->PathInfo = const_cast<char*>(PATH_INFO.c_str());
    pipe(Client->fd);
    Client->c_pid = fork();
    char *env[] = {Client->PathInfo, 0};
    char *args[] = {Client->Arg0, Client->realPATH, 0};
    if (Client->c_pid == -1) return 500;
    else if (Client->c_pid == 0) {
        dup2(Client->fd[1], 1);
        execve(args[0], args, env);
        return 2;
    }
    return 1;
}

bool Response::look_for_BWS(std::string field_name) {
    for(std::string::iterator it = field_name.begin(); it != field_name.end();it++){
        if (isspace((*it)) && (*it) != '\r' && (*it) != '\n')
            return true;
    }
    return false;
}

int Response::ParseCGIHeaders(char* buffer) {
    std::stringstream getHeaders;
    getHeaders << buffer;
    std::string key_value_geminie, key, value;
    int index_offset = 0;
    RequestHeader.clear();
    while(std::getline(getHeaders,key_value_geminie)){
        if (key_value_geminie.find("\r\n\r\n") != std::string::npos)
            return 0;
        if (key_value_geminie.find(":") == std::string::npos)
            return 1;
        index_offset = key_value_geminie.find(":") + 1;
        key = key_value_geminie.substr(0,index_offset - 1);
        value = key_value_geminie.substr(index_offset + 1);
        value.pop_back();
        if (key == "Content-type" && value.find(";") != std::string::npos)
            value = value.substr(0, value.find(";"));
        if (look_for_BWS(key) || isspace(value[0]))
            return 1;
        RequestHeader.insert(std::make_pair(key,value)); 
    }
    return 0;
}

int Response::IfLocationHaveCGI(Client_Gymir* Client, Server_Master& Server) {
    if (Server.conf.Servers[LocationIndex->first].Locations[LocationIndex->second]->CGI->size() == 0) return StatusCode;
    if (Client->PathInfo == NULL) {
        if (InitCGI(Client, Server.conf) == 0)
            return StatusCode;
        if (InitCGI(Client, Server.conf) == 2)
            return 404;
    }
    int wait = waitpid(Client->c_pid, NULL, WNOHANG);
    if (wait == -1) return 400;
    if (wait == 0) return -1;
    char buffer[100000];
    int r = read(Client->fd[0], buffer, 10000);
    buffer[r] = '\0';
    close(Client->fd[1]);
    close(Client->fd[0]);
    StatusCode = ParseCGIHeaders(buffer);
    StatusCode = ParseBody(buffer, Server);
    return StatusCode;
}

int Response::ParseBody(char* buffer, Server_Master& Server) {
    std::stringstream CgiOutput;
    CgiOutput << buffer;
    std::string buf = CgiOutput.str();
    if (buf.find("\r\n\r\n") == std::string::npos) return 400;
    Header = buf.substr(0, buf.find("\r\n\r\n"));
    buf = buf.substr(buf.find("\r\n\r\n") + 4);
    char* myfile = const_cast<char*>(buf.c_str());
    File.open("/tmp/fileCGI" + Server.getReverseContentType(RequestHeader.at("Content-type").c_str()));
    File << myfile;
    Path = "/tmp/fileCGI" + Server.getReverseContentType(RequestHeader.at("Content-type").c_str());
    File.close();
    return 200;
}
    // std::string REQUEST_METHOD("REQUEST_METHOD=");
    // REQUEST_METHOD.append(HTTPMethod);
    // strcpy(RequestMethod, REQUEST_METHOD.c_str());

    // std::string REDIRECT_STATUS("REDIRECT_STATUS=");
    // REDIRECT_STATUS.append("200");
    // strcpy(Status, REDIRECT_STATUS.c_str());

    // std::string HTTP_COOKIE("HTTP_COOKIE=");
    // if (RequestHeader["cookie"].size())
    //     HTTP_COOKIE.append(RequestHeader["cookie"]);
    // strcpy(HttpCookie, HTTP_COOKIE.c_str());

    // std::string QUERY_STRING("QUERY_STRING=");
    // if (Path.find("?") != std::string::npos)
    //     QUERY_STRING.append(Path.substr(Path.find("?")));
    // strcpy(QueryString, QUERY_STRING.c_str());

    // std::string CONTENT_LENGTH("CONTENT_LENGTH=");
    // CONTENT_LENGTH.append(RequestHeader["Content-Length"]);
    // strcpy(ContentLength, CONTENT_LENGTH.c_str());

    // std::string CONTENT_TYPE("CONTENT_TYPE=");
    // CONTENT_TYPE.append(RequestHeader["Content-Type"]);
    // strcpy(ContentType, CONTENT_TYPE.c_str());