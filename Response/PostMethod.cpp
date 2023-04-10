#include "../Client_Gymir.hpp"
#include "../Server_Master.hpp"

std::string	get_time()
{
	struct timeval	t0;
	gettimeofday(&t0, NULL);
	return std::to_string(t0.tv_sec * 1000 + t0.tv_usec / 1000);
}


int Response::PostMethod(Client_Gymir* & Client, Server_Master& Server, std::string OldPath) {
    StatusCode = getResourcePath(Server.conf);
    if (StatusCode != 200) return StatusCode;
    if (getResourceType()) { // if it's directory
        StatusCode = IsURIHasSlashAtTheEnd(OldPath);
        if (StatusCode != 200) return StatusCode;
        // StatusCode = IsDirHaveIndexFiles(Server.conf);
        // if (StatusCode != 200) return StatusCode;
    }
    if (IfLocationHaveCGI(Client, Server) == 0) return StatusCode;
    struct stat sb;
    std::string NewPath = Path;
    NewPath.append(get_time().c_str() + Client->FilePath.substr(Client->FilePath.find_last_of(".")));
    rename(Client->FilePath.c_str(), NewPath.c_str());
    return 201;
}