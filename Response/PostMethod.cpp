#include "../Client_Gymir.hpp"
#include "../Server_Master.hpp"

int Response::PostMethod(Client_Gymir* & Client, Server_Master& Server, std::string OldPath) {
    StatusCode = getResourcePath(Server.conf);
    if (StatusCode != 200) return StatusCode;
    if (getResourceType()) { // if it's directory
        StatusCode = IsURIHasSlashAtTheEnd(OldPath);
        if (StatusCode != 200) return StatusCode;
        StatusCode = IsDirHaveIndexFiles(Server.conf);
        if (StatusCode != 200) return StatusCode;
    }
    if (IfLocationHaveCGI(Client, Server) == 0) return StatusCode;
    return 201;
}