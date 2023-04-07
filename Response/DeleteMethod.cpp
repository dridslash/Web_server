#include "Response.hpp"
// #include "../Derya_Request.hpp"
#include "../Config/ConfigFile.hpp"
// #include "../Client_Smár.hpp"
// #include "../Server_Eyjafjörður.hpp"

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