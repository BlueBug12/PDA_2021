#include <iostream>
#include "FM.hpp"

int main(int argc, char ** argv){
    std::string out_name;
    if(argc == 2){
     out_name = "output.txt";
    }else if(argc ==3){
     out_name = argv[2];
    }else{
     std::cerr << "Error: wrong parameters" << std::endl;
     exit(1);
    }

    float balance_ratio = 0.45;
    FM fm(argv[1], balance_ratio);
    fm.preprocess2(2);
    fm.run();
    fm.writeOutput(out_name);
    return 0;
}
