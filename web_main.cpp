#include "Server_Master.hpp"

int main(int ac, char **argv){

    Server_Master Main_server;

    Main_server.Upp_ports(argv[1]);
    Main_server.multiplexing();
}