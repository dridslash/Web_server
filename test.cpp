#include <unistd.h>
#include <iostream>
#include <string>

int main(int argc, char **argv) {

    std::string path = "/cgiprogramform";
    std::string locationpath = "/cgiprogramform";

    std::cout << path.find(locationpath) << std::endl;


    return 0;
}