#include <iostream>
#include "sp.hpp"

int main(int argc, char **argv){
    if(argc != 5){
        std::cerr<<"Error: wrong input parameters."<<std::endl;
        exit(1);
    }
    float alpha = std::stof(argv[1]);
    std::string block_name = argv[2];
    std::string net_name = argv[3];
    std::string output_name = argv[4];

    SP sp(block_name, net_name);
    std::vector<int>X = {3,2,0,5,1,4};
    std::vector<int>Y = {5,2,4,3,0,1};
    sp.setInitial(X,Y);
    sp.LCS();

    return 0;
}
