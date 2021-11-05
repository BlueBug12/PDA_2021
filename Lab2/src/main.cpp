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
    clock_t start = clock();
    clock_t temp;
    FM fm(argv[1], balance_ratio);
    fm.initialize();
    std::cout<<"Initialization takes "<<double(clock()-start)/CLOCKS_PER_SEC<<" s."<<std::endl;
    while(!fm.stop){
        start = clock();
        std::cout<<"Start round "<<counter<<std::endl;
        temp = clock();
        for(size_t i=0;i<fm.cell_num;++i){
            Cell *c = fm.chooseCell();
            fm.updateGain(c);
        }
        std::cout<<"UpdateGain takes "<<double(clock()-temp)/CLOCKS_PER_SEC<<" s."<<std::endl;
        temp = clock(); 
        fm.storeResult();
        std::cout<<"storeResult takes "<<double(clock()-temp)/CLOCKS_PER_SEC<<" s."<<std::endl;

        temp = clock();     
        fm.unlockAll();
        std::cout<<"unlockAll takes "<<double(clock()-temp)/CLOCKS_PER_SEC<<" s."<<std::endl;

        temp = clock();
        fm.initialize();
        std::cout<<"initialize takes "<<double(clock()-temp)/CLOCKS_PER_SEC<<" s."<<std::endl;

        std::cout<<"Round "<<counter++<<" takes "<<double(clock()-start)/CLOCKS_PER_SEC<<" s."<<std::endl<<std::endl;
    }
    temp = clock();
    fm.writeOutput(out_name);
    std::cout<<"writeOutput takes "<<double(clock()-temp)/CLOCKS_PER_SEC<<" s."<<std::endl;
    std::cout<<"Total takes "<<double(clock()-start)/CLOCKS_PER_SEC<<" s."<<std::endl;

    std::cout<<fm.case1<<std::endl;
    std::cout<<fm.case2<<std::endl;
    std::cout<<fm.case3<<std::endl;
    std::cout<<fm.case4<<std::endl;
   return 0;
}
