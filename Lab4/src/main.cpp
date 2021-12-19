#include <iostream>
#include "abacus.hpp"


int main(int argc, char ** argv){
    if(argc!=2){
        std::cerr<<"Error: wrong parameters."<<std::endl;
        exit(1);
    }
    Abacus *ab = new Abacus(argv[1]);
    ab->run();
    ab->writeOutput("output.pl");
    delete ab;
    return 0;
}
