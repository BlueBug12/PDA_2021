#include <iostream>
#include <chrono>
#include "sa.hpp"

int main(int argc, char **argv){
    clock_t start = clock();

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
    int markov_iter = 100000;

    SA sa(descet_rate,initial_t,final_t,markov_iter);
    
    float period = 0.f;
    float timer = 300.f;
    int i = 0;
    while(timer -(float)(clock()-start)/CLOCKS_PER_SEC > 3*period){
        clock_t it_beg = clock();
        sa.buildSP(block_name, net_name, alpha);
        sa.run();
        sa.updateResult();
        clock_t it_end = clock();
#ifdef DISPLAY
        std::cout<<"round "<<++i <<" takes "<< (float)(it_end-it_beg)/CLOCKS_PER_SEC<<std::endl<<std::endl;
#endif
        period = std::max((float)(it_end-it_beg)/CLOCKS_PER_SEC,period);
    }
    sa.writeResult(output_name,std::min((float)298.8,(float)(clock()-start)/CLOCKS_PER_SEC));
#ifdef DISPLAY
    std::cout<<"Final cost:"<<sa.global_cost<<std::endl;
    std::cout<<"Final width:"<<sa.global_width<<" (outline_w = "<<(sa.sp)->outline_w<<")"<<std::endl;
    std::cout<<"Final height:"<<sa.global_height<<" (outline_h = "<<(sa.sp)->outline_h<<")"<<std::endl;
    std::cout<<"Final area:"<<sa.global_area<<std::endl;
    std::cout<<"Final hpwl:"<<sa.global_hpwl<<std::endl;
    std::cout<<"Total takes "<<(float)(clock()-start)/CLOCKS_PER_SEC<<" s."<<std::endl;
#endif
    return 0;
}
