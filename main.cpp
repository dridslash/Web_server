#include "Server_Master.hpp"

int main(int argc, char **argv){

    if (argc != 2) {
        std::cout << "Invalid Arguments" << std::endl;
        return -1;
    }
    Server_Master Main_server;

    Main_server.Upp_ports(argv[1]);
    Main_server.multiplexing();
    return 0;
}