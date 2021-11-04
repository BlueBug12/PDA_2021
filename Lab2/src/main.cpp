#include <iostream>
#include "data_struct.hpp"
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
    int counter = 1;
    FM fm(argv[1], balance_ratio);
    fm.initialize();
    while(!fm.stop){
        std::cout<<"Start round "<<counter++<<std::endl;
        for(size_t i=0;i<fm.cell_num;++i){
            Cell *c = fm.chooseCell();
            fm.updateGain(c);
        }
        fm.storeResult();
        fm.unlockAll();
        fm.initialize();
    }
    fm.writeOutput(out_name);

   return 0;
}
