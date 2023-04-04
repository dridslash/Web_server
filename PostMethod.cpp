#include "Response.hpp"
#include "Derya_Request.hpp"
#include "ConfigFile.hpp"
#include "Client_Smár.hpp"
#include "Server_Eyjafjörður.hpp"

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