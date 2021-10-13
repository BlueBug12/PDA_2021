#include <iostream>
#include <fstream>
#include "corner_stitching.hpp"
using std::cout;
using std::endl;
using std::cerr;

int main(int argc, char ** argv){
    if(argc != 3){
        cerr << "Error: wrong parameters" << endl;
        exit(1);
    }
    std::ifstream fin{argv[1]};
    if(!fin){
        cerr << "Error: can not open file "<< argv[1] << endl;
        exit(1);
    }
    int width, height, x, y;
    fin >> width >> height;
    CornerStitching cs(width, height);
    std::string temp;
    while(fin>>temp){
        if(temp=="P"){
            fin >> x >> y;
            Tile* t = cs.findPoint(x,y);
            std::cout<<t->x<<" "<<t->y<<std::endl;
        }else{
            fin >> x >> y >> width >> height;
            cs.insertTile(x,y,width,height,std::stoi(temp));
        }
    }
    return 0;
}
