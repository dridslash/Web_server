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
        StatusCode = IsDirHaveIndexFiles(Server.conf);
        if (StatusCode != 200) return StatusCode;
    }
    struct stat sb;
    if (stat(Client->FilePath.c_str(), &sb) == 0) {
        NewPath = Server.conf.Servers[LocationIndex->first].Locations[LocationIndex->second]->upload_pass + "/";
        if (stat(NewPath.c_str(), &sb)) {
            if (Server.conf.Servers[LocationIndex->first].Locations[LocationIndex->second]->root.size())
                NewPath = Server.conf.Servers[LocationIndex->first].Locations[LocationIndex->second]->root + "/";
            else
                NewPath = Server.conf.Servers[LocationIndex->first].root + "/";
        }
        std::string PathFile = Client->FilePath.substr(2);
        NewPath.append(get_time().c_str());
        if (PathFile.find(".") != std::string::npos)
            NewPath.append(PathFile.substr(PathFile.find(".")));
        rename(Client->FilePath.c_str(), NewPath.c_str());
        remove(Client->FilePath.c_str());
    }
    StatusCode = HandleCGIprogram(Client, Server);
    return StatusCode;
}