#include <sstream>
#include <netdb.h>
#include <sys/event.h>
#include <sys/time.h>
#include <map>
#include <array>
#include <utility>
#include <list>
#include <set>
#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <fcntl.h>
#include <cstdio>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>
#include <limits.h>

#define BUFFER_SIZE 1024
#define MAX_CONNECTIONS 1024

#define Dvergmál(a) std::cout << a << std::endl;
#define CLOSING_SOCKET(s) close(s)
