#include "Response.hpp"
#include "../Derya_Request.hpp"
#include "../Config/ConfigFile.hpp"
#include "../Client_Gymir.hpp"
#include "../Server_Master.hpp"

int Response::DeleteMethod(Client_Gymir* & Client, Server_Master& Server, std::string OldPath) {
    (void)Client;
    StatusCode = getResourcePath(Server.conf);
    if (StatusCode != 200) return StatusCode;
    if (getResourceType()) { // if it's directory
        StatusCode = IsURIHasSlashAtTheEnd(OldPath);
        if (StatusCode != 200) return StatusCode;
        if (access(Path.c_str(), W_OK) == -1) return 403;
        StatusCode = RemoveDirectory(Path.c_str());
        if (StatusCode != 204) return StatusCode;
    }
    else if (remove(Path.c_str()) == -1) {
        if (access(Path.c_str(), R_OK) == -1) return 500;
        return 403;
    }
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
                if (access(Path.c_str(), W_OK) == -1) return 403;
                StatusCode = RemoveDirectory(Path);
                if (StatusCode != 204) return StatusCode;
            }
        } else {
            if (remove(Path.c_str()) == -1) {
                if (access(Path.c_str(), R_OK) == -1) return 500;
                return 403;
            }
        }
    }
    Path = Path.substr(0, Path.find_last_of("/") + 1);
    if (path == Path) {
        Path = Path.substr(0, Path.find_last_of("/"));
        if (rmdir(Path.c_str()) == -1)
            return 500;
    }
    return 204;
}