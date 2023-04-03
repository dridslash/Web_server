#include "Server_Eyjafjörður.hpp"

int main(int ac, char **argv){

    Server_Eyjafjörður Main_server;

    Main_server.Upp_ports(argv[1]);
    Main_server.multiplexing();
}