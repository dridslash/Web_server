#pragma once

#include <sstream>
#include <netdb.h>
#include <sys/event.h>
#include <sys/time.h>
#include <map>
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
#include <poll.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/event.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <deque>
#include <map>
#include <array>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>
#include <limits.h>


#define BUFFER_SIZE 1000000
#define MAX_CONNECTIONS 200


#define Dvergmál(a) std::cout << a << std::endl
#define CLOSING_SOCKET(s) close(s)