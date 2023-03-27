#include "ConfigFile.hpp"

LocationBlocks::LocationBlocks() : AutoIndex("off") {
     httpmethods = new std::vector<std::string>();
    index = new std::vector<std::string>();
    try_files = new std::vector<std::string>();
    Return = new std::vector<std::string>();
    CGI = new std::map<std::string, std::string>();
}

// LocationBlocks::~LocationBlocks() {
//     delete httpmethods;
//     delete index;
//     delete try_files;
//     delete Return;
// }

void LocationBlocks::setAutoIndex(std::string str) { AutoIndex = str; }
void LocationBlocks::setRoot(std::string str) { root = str; }
void LocationBlocks::setMethods(std::vector<std::string> str) { httpmethods->assign(str.begin(), str.end()); }
void LocationBlocks::setCGI(std::pair<std::string, std::string> str) { CGI->insert(str); }
void LocationBlocks::setIndex(std::vector<std::string> str) { index->assign(str.begin(), str.end()); }
void LocationBlocks::setTryFiles(std::vector<std::string> str) { try_files->assign(str.begin(), str.end()); }
void LocationBlocks::setReturn(std::vector<std::string> str) { Return->assign(str.begin(), str.end()); }


ServerBlocks::ServerBlocks() : listen(1, "80") {
    std::set<int> s;
    s.insert(400);
    ErrorPage.insert(std::make_pair(s, "/Errors/400.html"));
    s.clear(); s.insert(403);
    ErrorPage.insert(std::make_pair(s, "/Errors/403.html"));
    s.clear(); s.insert(404);
    ErrorPage.insert(std::make_pair(s, "/Errors/404.html"));
    s.clear(); s.insert(405);
    ErrorPage.insert(std::make_pair(s, "/Errors/405.html"));
    s.clear(); s.insert(409);
    ErrorPage.insert(std::make_pair(s, "/Errors/409.html"));
    s.clear(); s.insert(413);
    ErrorPage.insert(std::make_pair(s, "/Errors/413.html"));
    s.clear(); s.insert(414);
    ErrorPage.insert(std::make_pair(s, "/Errors/414.html"));
    s.clear(); s.insert(500);
    ErrorPage.insert(std::make_pair(s, "/Errors/500.html"));
    s.clear(); s.insert(501);
    ErrorPage.insert(std::make_pair(s, "/Errors/501.html"));
}
void ServerBlocks::setIndex(std::vector<std::string> vec) { index = vec; }
void ServerBlocks::setErrorPage(std::pair<std::set<int>, std::string> vec) { 
    std::map<std::set<int>, std::string>::iterator found = ErrorPage.find(vec.first);
    if (found != ErrorPage.end())
        ErrorPage.erase(ErrorPage.find(vec.first));
    ErrorPage.insert(vec);
}
void ServerBlocks::setServerName(std::string str) { ServerName = str;}
void ServerBlocks::setRoot(std::string str) { root = str;}


Config::Config() : MaxBodySize("1000000") {
    std::string myArray[] = {"listen", "server_name", "root", "index", "error_page", "location"};
    Directives.insert(myArray, myArray + 6);
}

Config::ErrorBox    Config::ServerBlock(ServerBlocks & server, std::vector<std::string> & Store, size_t & i) {
    ErrorBox r;
    int k = 0;
    if (Store[i] == "server" && Store[i+1] == "{" && Directives.find(Store[i+2]) != Directives.end())
        i += 2;
    else if (Store[i] == "server{" &&  Directives.find(Store[i+1]) != Directives.end())
        i++;
    else return std::make_pair(std::make_pair(Store[i], Store[i]), 1);
    while (Store[i] != "}") {
        if (Store[i] == "error_page" || Store[i] == "index") {
            r = DirectivesMoreThanOneValue(NULL, server, Store, i, 0);
            if (r.second) return r;
        } else if (Store[i] == "server_name" || Store[i] == "listen" || Store[i] == "root") {
            r = DirectivesOneValue(NULL, server, Store, i, 0);
            if (r.second) return r;
        }
        else {
            LocationBlocks* location = new LocationBlocks();
            r = LocationBlock(location, server, Store, i);
            if (r.second) return r;
            k++;
        }
    }
    // delete location;
    return std::make_pair(std::make_pair("NO", "ERROR"), 0);
}

Config::ErrorBox    Config::LocationBlock(LocationBlocks * location, ServerBlocks & server, std::vector<std::string> & Store, size_t & i) {
    ErrorBox r;
    if (Store[i] == "location" && Store[i+1] != "{" && Store[i+1] != "}" && Store[i+2] == "{") {
        location->path = Store[i+1];
        i += 3;
    }
    else  return std::make_pair(std::make_pair(Store[i], Store[i]), 1);
    while (Store[i] != "}") {
        if (Store[i] == "cgi_param" || Store[i] == "try_files" || Store[i] == "httpmethods" || Store[i] == "return" || Store[i] == "index") {
            r = DirectivesMoreThanOneValue(location, server, Store, i, 1);
            if (r.second) return r;
        } else if (Store[i] == "root" || Store[i] == "autoindex") {
            r = DirectivesOneValue(location, server, Store, i, 1);
            if (r.second) return r;
        }
    }
    i++;
    server.Locations.push_back(location);
    return std::make_pair(std::make_pair("GOOD", "JOB"), 0);
}

Config::ErrorBox    Config::DirectivesMoreThanOneValue(LocationBlocks * location, ServerBlocks & server, std::vector<std::string> & Store, size_t & i, int FLAG) {
    std::vector<std::string> Values;
    std::string Directive;
    size_t found = Store[i+1].find(';');
    if (!found) return std::make_pair(std::make_pair(Store[i], Store[i]), 1);
    std::string IstillHave;
    if (FLAG == 0) {
        void (ServerBlocks::*arr[2])( std::vector<std::string> ) = {NULL, &ServerBlocks::setIndex};
        int Dir = (Store[i] == "index") * 1 + (Store[i] == "error_page") * 0;
        (Dir == 1) ? Directive = "index" : Directive = "error_page";
        // if their is only one value, then the Dir is index
        if (found != std::string::npos) {
            if (!Dir) return std::make_pair(std::make_pair(Store[i], Store[i]), 1);
            Values.push_back(Store[++i].substr(0, found));
            IstillHave = Store[i].substr(found + 1, Store[i].length());
            (server.*arr[Dir])(Values);
        } else {
            // if their is more than one value, then
            while (Store[++i].find(';') == std::string::npos) {
                if (!Dir && Store[i].size() != strspn(Store[i].c_str(), "0123456789")) return std::make_pair(std::make_pair("error_page", Store[i]), 1);
                if (!Dir && (stoi(Store[i]) < 300 || stoi(Store[i]) > 599)) return std::make_pair(std::make_pair("error_page", Store[i]), -2);
                if (Directives.find(Store[i]) != Directives.end()) return std::make_pair(std::make_pair(Directive, Store[i]), 1);
                Values.push_back(Store[i]);
            }
            if (Store[i].find(';') == 0) return std::make_pair(std::make_pair(Store[i-1], Store[i]), 1);
            Values.push_back(Store[i].substr(0, Store[i].find(';')));
            IstillHave = Store[i].substr(Store[i].find(';') + 1, Store[i].length());
            if (!Dir) {
                std::set<int> ValueOfSet;
                for (size_t i = 0; i < Values.size() - 1; i++) {
                    if (Values[i].size() != strspn(Values[i].c_str(), "0123456789")) return std::make_pair(std::make_pair(Store[i-1], Store[i]), 1);
                    int var = stoi(Values[i]);
                    if (var < 300 || var > 599) return std::make_pair(std::make_pair(Store[i+1], Store[i]), -2);
                    ValueOfSet.insert(var);
                }
                server.setErrorPage(std::make_pair(ValueOfSet, Values.back()));
            }
            else (server.*arr[Dir])(Values);
        }
    } else {
        if (Store[i] == "cgi_param") {
            if (found != std::string::npos) return std::make_pair(std::make_pair(Store[i], Store[i]), 1);
            if (Store[i+2].find(';') == std::string::npos)  return std::make_pair(std::make_pair(Store[i], Store[i]), 1);
            location->setCGI(std::make_pair(Store[i+1], Store[i+2].substr(0, Store[i+2].find(';'))));
            i += 3;
            return std::make_pair(std::make_pair("GOOD", "JOB"), 0);
        }
        void (LocationBlocks::*arr[4])( std::vector<std::string> ) = {&LocationBlocks::setIndex, &LocationBlocks::setMethods, &LocationBlocks::setTryFiles, &LocationBlocks::setReturn};
        int Dir = (Store[i] == "index") * 0 + (Store[i] == "httpmethods") * 1 + (Store[i] == "try_files") * 2 + (Store[i] == "return") * 3;
        // if their is only one value, then the Dir is index
        if (found != std::string::npos) {
            if (Dir > 1) return std::make_pair(std::make_pair(Store[i-1], Store[i]), 1);
            Values.push_back(Store[++i].substr(0, found));
            IstillHave = Store[i].substr(found + 1, Store[i].length());
            (location->*arr[Dir])(Values);
        } else {
            // if their is more than one value, then
            while (Store[++i].find(';') == std::string::npos) {
                if (Directives.find(Store[i]) != Directives.end()) return std::make_pair(std::make_pair(Store[i-1], Store[i]), 1);
                Values.push_back(Store[i]);
            }
            Values.push_back(Store[i].substr(0, Store[i].find(';')));
            IstillHave = Store[i].substr(Store[i].find(';') + 1, Store[i].length());
            if (Dir == 3) {
                if (Values.size() > 2) return std::make_pair(std::make_pair("return", Store[i-1]), 1);
                if (Values[0].size() != strspn(Values[0].c_str(), "0123456789")) return std::make_pair(std::make_pair(Store[i-2], Store[i-1]), 1);
                if (stoi(Values[0]) < 0 || stoi(Values[0]) > 999) return std::make_pair(std::make_pair(Store[i-2], Store[i-1]), 4);
            }
            if (Dir == 1) {
                for (size_t i = 1; i < Values.size(); i++) {
                    if (Values[i] != "POST" && Values[i] != "DELETE" && Values[i] != "GET") return std::make_pair(std::make_pair(Store[i-1], Store[i]), 1);
                }
            }
            (location->*arr[Dir])(Values);
        }
    }
    if (IstillHave.length() == 0) i++;
    else Store[i] = IstillHave;
    return std::make_pair(std::make_pair("GOOD", "JOB"), 0);
}
Config::ErrorBox    Config::DirectivesOneValue(LocationBlocks * location, ServerBlocks & server, std::vector<std::string> & Store, size_t & i, int FLAG) {
    size_t found = Store[++i].find(';');
    if (!found) return std::make_pair(std::make_pair(Store[i-1], Store[i]), 1);
    if (found == std::string::npos && Directives.find(Store[i]) != Directives.end()) return std::make_pair(std::make_pair(Store[i-1], Store[i]), 1);
    if (found == std::string::npos) return std::make_pair(std::make_pair(Store[i-1], Store[i]), 1);
    std::string Value = Store[i].substr(0, found);
    std::string IstillHave = Store[i].substr(found + 1, Store[i].length());
    if (FLAG == -1) {
        if (Value.size() != strspn(Value.c_str(), "0123456789")) return std::make_pair(std::make_pair(Store[i-1], Value), 5);
        MaxBodySize = Value;
    }
    else if (FLAG == 0) {
        void (ServerBlocks::*arr[4])( std::string ) = {NULL, &ServerBlocks::setServerName, &ServerBlocks::setRoot};
        int Dir = (Store[i-1] == "listen") * 0 + (Store[i-1] == "server_name") * 1 + (Store[i-1] == "root") * 2;
        if (Dir == 2 && (Value != "./public" && Value == "/public" && Value == "public")) return std::make_pair(std::make_pair(Store[i-1], Store[i]), 1);
        else if (Dir) (server.*arr[Dir])(Value);
        else {
            if (Value.size() != strspn(Value.c_str(), "0123456789")) return std::make_pair(std::make_pair(Store[i-1], Store[i]), 1);
            if (stoi(Value) > 65535 || stoi(Value) <= 0) return std::make_pair(std::make_pair(Store[i-1], Value), 3);
            server.listen.push_back(Value);
            Ports.insert(Value);
        }
    } else {
        void (LocationBlocks::*arr[2])( std::string ) = {&LocationBlocks::setRoot, &LocationBlocks::setAutoIndex};
        int Dir =(Store[i-1] == "root") * 0 + (Store[i-1] == "autoindex") * 1;
        if (Dir && Value != "on" && Value != "off") return std::make_pair(std::make_pair(Store[i-1], Value), 2);
        (location->*arr[Dir])(Value);
    }
    if (IstillHave.length() == 0) i++;
    else Store[i] = IstillHave;
    return std::make_pair(std::make_pair("GOOD", "JOB"), 0);
}

void Config::HandleErrors(ServerBlocks & server, Config::ErrorBox ErrorMsg) {
    switch (ErrorMsg.second) {
        case 1:
            printf("invalid number of arguments in a \"%s\" directive\n", ErrorMsg.first.first.c_str());
            exit(1);
        case 2:
            printf("invalid value \"%s\" in \"%s\" directive , it must be \"on\" or \"off\"\n", ErrorMsg.first.second.c_str(), ErrorMsg.first.first.c_str());
            exit(1);
        case -2:
            printf("invalid value \"%s\" in \"%s\" directive must be between 300 and 599\n", ErrorMsg.first.second.c_str(), ErrorMsg.first.first.c_str());
            exit(1);
        case 3:
            printf("invalid port in \"%s\" of the \"%s\" directive\n", ErrorMsg.first.second.c_str(), ErrorMsg.first.first.c_str());
            exit(1);
        case 4:
            printf("invalid return code \"%s\"\n", ErrorMsg.first.second.c_str());
            exit(1);
        case 5:
            printf("\"%s\" directive invalid value\n", ErrorMsg.first.first.c_str());
            exit(1);
        default:
            server.listen.erase(server.listen.begin());
            Servers.push_back(server);
            break;
    }
}

int    Config::ConfigParse(char *ConfigPath) {
    ErrorBox r;
    std::string Line, resp, text;
    std::stringstream ss;
    std::vector<std::string> Store;
    std::ifstream ConfigFile(ConfigPath);
	std::getline(ConfigFile, Line);
    while ( ConfigFile ) { 
        ss << Line;
        size_t found = Line.find_first_not_of(" \t");
        if (found < Line.size() && Line[found] != '#') {
            resp.append(Line);
		    resp.append(1, '\n');
        }
		std::getline(ConfigFile, Line);
    }
    ss.str("");
    ss << resp;
    while(ss) {
        ss >> Line;
        Store.push_back(Line);
    }
    Store.pop_back();
    // Server Block
    for (size_t i = 0; i < Store.size(); i++) {
        ServerBlocks server;
        if (Store[i] == "client_max_body_size")
            r = DirectivesOneValue(NULL, server, Store, i, -1);
        if(r.second == 0)
            r = ServerBlock(server, Store, i);
        HandleErrors(server, r);
    }
    for (std::vector<ServerBlocks>::iterator it = Servers.begin(); it != Servers.end(); it++) {
        if (it->index.size() == 0) {
            it->index.push_back("index.html");
        }
        if (it->ErrorPage.size() == 0) {
            std::set<int> SetContainer;
            SetContainer.insert(404);
            it->setErrorPage(std::make_pair(SetContainer, "/Error/404.html"));
        }
    }
    return 0;
}