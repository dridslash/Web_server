#include "Response.hpp"
#include "../Derya_Request.hpp"
#include "../Config/ConfigFile.hpp"
#include "../Client_Smár.hpp"
#include "../Server_Eyjafjörður.hpp"

int Response::IfLocationHaveCGI(Config config) {
    if (config.Servers[LocationIndex->first].Locations[LocationIndex->second]->CGI->size() == 0) return 0;
    char *PathInfo, Arg0[PATH_MAX];
    std::string exec;
    if (Path.find(".php") != std::string::npos) {
        exec = config.Servers[LocationIndex->first].Locations[LocationIndex->second]->CGI->at("php");
        exec.insert(0, "/");
        if (config.Servers[LocationIndex->first].Locations[LocationIndex->second]->root.size())
            exec.insert(0, config.Servers[LocationIndex->first].Locations[LocationIndex->second]->root);
        else
            exec.insert(0, config.Servers[LocationIndex->first].root);
        realpath(exec.c_str(), Arg0);
    }
    else if (Path.find(".pl") != std::string::npos) {
        exec = config.Servers[LocationIndex->first].Locations[LocationIndex->second]->CGI->at("pl");
        strcpy(Arg0, exec.c_str());
    }
    else
        return 0;
    char realPATH[PATH_MAX];
    realpath(Path.c_str(), realPATH);
    std::string PATH_INFO("PATH_INFO=");
    PATH_INFO.append(realPATH);
    PathInfo = const_cast<char*>(PATH_INFO.c_str());

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

    char *env[] = {PathInfo, 0};
    char *args[] = {Arg0, realPATH, 0};
    int fd[2];
    pipe(fd);
    pid_t c_pid = fork();
    if (c_pid == -1) return 500;
    else if (c_pid == 0) {
        dup2(fd[1], 1);
        execve(args[0], args, env);
        return 400;
    }
    int wait = waitpid(c_pid, NULL, WNOHANG);
    if (wait == -1) return 400;
    if (wait == 0) return -1;
    std::cout << "/* cursor */" << std::endl;
    char buffer[100000];
    int r = read(fd[0], buffer, 10000);
    buffer[r] = '\0';
    close(fd[1]);
    close(fd[0]);
    StatusCode = ParseCgiOutput(buffer);
    return 1;
}

int Response::ParseCgiOutput(char* buffer) {
    std::stringstream CgiOutput;
    CgiOutput << buffer;
    std::string buf = CgiOutput.str();
    if (buf.find("\r\n\r\n") == std::string::npos) return 400;
    Header = buf.substr(0, buf.find("\r\n\r\n"));
    buf = buf.substr(buf.find("\r\n\r\n") + 4);
    char* myfile = const_cast<char*>(buf.c_str());
    File.open("/tmp/fileCGI.html");
    File << myfile;
    Path = "/tmp/fileCGI.html";
    File.close();
    return 200;
}