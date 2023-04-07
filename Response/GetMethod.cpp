#include "Response.hpp"
// #include "../Derya_Request.hpp"
#include "../Config/ConfigFile.hpp"
// #include "../Client_Smár.hpp"
// #include "../Server_Eyjafjörður.hpp"

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