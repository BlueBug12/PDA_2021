#include <iostream>
#include "sa.hpp"

int main(int argc, char **argv){
    if(argc != 5){
        std::cerr<<"Error: wrong input parameters."<<std::endl;
        exit(1);
    }
    float alpha = std::stof(argv[1]);
    std::string block_name = argv[2];
    std::string net_name = argv[3];
    std::string output_name = argv[4];
    
    double descet_rate = 0.98;
    double initial_t = 100.0;
    double final_t = 1.0;
    double scale = 0.5;
    int markov_iter = 100;
    double scale_descen_rate = 0.99;

    SA sa(descet_rate,initial_t,final_t,scale,markov_iter,scale_descen_rate);
    sa.buildSP(block_name, net_name, alpha);
    sa.run();
    sa.writeResult(output_name);
    //SP sp(block_name, net_name, alpha);
    /*
    std::vector<int>X = {3,2,0,5,1,4};
    std::vector<int>Y = {5,2,4,3,0,1};
    sp.setInitial(X,Y);*/

    return 0;
}
