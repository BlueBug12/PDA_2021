#include <iostream>
#include "sa.hpp"

int main(int argc, char **argv){

    float alpha;
    std::string block_name;
    std::string net_name;
    std::string output_name;

    if(argc == 4){
        alpha = std::stof(argv[1]);
        std::string folder = argv[2];
        std::string prefix = folder;
        prefix.pop_back();
        block_name = folder + prefix + ".block";
        net_name = folder + prefix + ".nets";
        output_name = argv[3];
    }else if(argc == 5){
        alpha = std::stof(argv[1]);
        block_name = argv[2];
        net_name = argv[3];
        output_name = argv[4];
    }else{
        std::cerr<<"Error: wrong input parameters."<<std::endl;
        exit(1);
    }
    
    double descet_rate = 0.7;
    double initial_t = 1000.0;
    double final_t = 1.0;
    double scale = 0.8;
    int markov_iter = 10000;
    double scale_descen_rate = 0.99;

    SA sa(descet_rate,initial_t,final_t,scale,markov_iter,scale_descen_rate);
    sa.buildSP(block_name, net_name, alpha);
    sa.run();
    sa.writeResult(output_name);

    return 0;
}
