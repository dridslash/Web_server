#include "../Client_Gymir.hpp"
#include "../Server_Master.hpp"


void Response::SetEnv(std::string& exec, Client_Gymir& Client, Config config) {
    Client.IsCGI = true;
    realpath(Path.c_str(), Client.realPATH);
    std::string PathInfo = "PATH_INFO=";
    PathInfo.append(Client.realPATH);
    std::string ScriptName = "SCRIPT_FILENAME=";
    ScriptName.append(Client.realPATH);
    envi[0] = PathInfo;
    envi[1] = "CONTENT_LENGTH=" + RequestHeader["Content-Length"];
    envi[2] = "CONTENT_TYPE=" + RequestHeader["Content-Type"];
    envi[3] = "REQUEST_METHOD=" + HTTPMethod;
    envi[4] = "QUERY_STRING=" + QueryString;
    envi[5] = "REDIRECT_STATUS=200";
    envi[6] = "HTTP_COOKIE=" + RequestHeader["Cookie"];
    envi[7] = ScriptName;

    ev[0] = const_cast<char*>(envi[0].c_str());
    ev[1] = const_cast<char*>(envi[1].c_str());
    ev[2] = const_cast<char*>(envi[2].c_str());
    ev[3] = const_cast<char*>(envi[3].c_str());
    ev[4] = const_cast<char*>(envi[4].c_str());
    ev[5] = const_cast<char*>(envi[5].c_str());
    ev[6] = const_cast<char*>(envi[6].c_str());
    ev[7] = const_cast<char*>(envi[7].c_str());
    ev[8] = 0;

    if (Path.find(".pl") != std::string::npos) {
        exec = config.Servers[LocationIndex->first].Locations[LocationIndex->second]->CGI->at("pl");
        strcpy(Client.Arg0, exec.c_str());
    }
    else {
        exec = config.Servers[LocationIndex->first].Locations[LocationIndex->second]->CGI->at("php");
        exec.insert(0, "/");
        if (config.Servers[LocationIndex->first].Locations[LocationIndex->second]->root.size())
            exec.insert(0, config.Servers[LocationIndex->first].Locations[LocationIndex->second]->root);
        else
            exec.insert(0, config.Servers[LocationIndex->first].root);
        realpath(exec.c_str(), Client.Arg0);
    }
}

int Response::HandleExec(Client_Gymir* Client, Config config) {
    std::string exec;
    SetEnv(exec, *Client, config);
    int InputFile = open(NewPath.c_str(), O_RDONLY, 0777);
    pipe(Client->fd);
    Client->c_pid = fork();
    char *env[] = {ev[0], ev[1], ev[2], ev[3], ev[4], ev[5], ev[6], ev[7], 0};
    char *args[] = {Client->Arg0, Client->realPATH, 0};
    if (Client->c_pid == -1) return 500;
    else if (Client->c_pid == 0) {
        dup2(Client->fd[1], 1);
        dup2(InputFile, 0);
        close(InputFile);
        execve(args[0], args, env);
        return 2;
    }
    return 1;
}

bool look_for_BWS(std::string field_name) {
    for(std::string::iterator it = field_name.begin(); it != field_name.end();it++){
        if (isspace((*it)) && (*it) != '\r' && (*it) != '\n')
            return true;
    }
    return false;
}

int Response::ParseCGIHeaders(std::string buffer) {
    std::stringstream getHeaders;
    getHeaders << buffer;
    std::string key_value_geminie, key, value;
    int index_offset = 0;
    RequestHeader.clear();
    while(std::getline(getHeaders,key_value_geminie)) {
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

int ReadBuffer(Client_Gymir& Client) {
    char buffer[Max_Reads + 1];
    int ReadReturn = read(Client.fd[0], buffer, Max_Reads);
    buffer[ReadReturn] = 0;
    Client.out.append(buffer);
    if (ReadReturn < Max_Reads)
        return 1;
    return 0;
}

int Response::HandleCGIprogram(Client_Gymir* Client, Server_Master& Server) {
    if (!Server.conf.Servers[LocationIndex->first].Locations[LocationIndex->second]->CGI->size())
        return StatusCode;
    if (!envi[0].size()) {
        if (HandleExec(Client, Server.conf) == 0) return StatusCode;
        if (HandleExec(Client, Server.conf) == 2) return 404;
    }
    int wait = waitpid(Client->c_pid, NULL, WNOHANG);
    if (Client->out.size() == 0 && wait == -1) return 400;
    if (Client->out.size() == 0 && wait == 0) return -1;
    if (ReadBuffer(*Client) == 0) return -1;
    close(Client->fd[1]);
    close(Client->fd[0]);
    StatusCode = ParseCGIHeaders(Client->out);
    StatusCode = ParseBody(Client->out, Server);
    return StatusCode;
}

int Response::ParseBody(std::string buffer, Server_Master& Server) {
    std::stringstream CgiOutput;
    CgiOutput << buffer;
    std::string buf = CgiOutput.str();
    if (buf.find("\r\n\r\n") == std::string::npos) return 400;
    Header = buf.substr(0, buf.find("\r\n\r\n"));
    buf = buf.substr(buf.find("\r\n\r\n") + 4);
    char* myfile = const_cast<char*>(buf.c_str());
    std::string FileName("/tmp/fileCGI");
    if (RequestHeader.find("Content-Type") != RequestHeader.end())
        FileName.append(Server.getReverseContentType(RequestHeader.at("Content-Type").c_str()));
    else if (RequestHeader.find("content-type") != RequestHeader.end())
        FileName.append(Server.getReverseContentType(RequestHeader.at("Content-Type").c_str()));
    File.open(FileName.c_str());
    File << myfile;
    Path = FileName;
    File.close();
    return 200;
}