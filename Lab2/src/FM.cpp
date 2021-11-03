#include "FM.hpp"

FM::FM(const std::string & file_name, float ratio){
   readInput(file_name);  
}

void FM::readInput(const std::string & file_name){
    std::ifstream fin{file_name};
    if(!fin){
        std::cerr << "Error: can not open file "<< file_name << std::endl;
        exit(1);
    }
    fin >> net_num >> cell_num;
    std::string line;
    while(std::getline(fin,line)){
        std::cout<<line<<std::endl;
    }
       


}
void FM::writeOutput(const std::string & file_name){}
void FM::initialPartition(){}
void FM::updateGain(Cell* c){}
Cell* FM::chooseCell(){
    return NULL;
} 
bool FM::checkBalance(Cell* c){
    return true;
}
void FM::unlockAll(){}
Cell* FM::findTarget(const int net_id, const bool side){
    return NULL;
}
void FM::storeResult(){
    return;    
}
