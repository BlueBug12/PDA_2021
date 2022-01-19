#include <iostream>
#include "greedy_channel_router.hpp"

int main(int argc, char **argv){
    std::string ifilename, ofilename;
    if(argc==2){
        ifilename = argv[1];
        ofilename = "output.txt";
    }else if(argc==3){
        ifilename = argv[1];
        ofilename = argv[2];
    }else{
        std::cerr << "Error: invalid parameters." << std::endl;
        exit(1);
    }

    GreedyCR *gcr = new GreedyCR(ifilename);
    gcr->run();
    gcr->writeOutput(ofilename);
    gcr->writeGDT("test.gdt");
    delete gcr;
    

    return 0;
}
