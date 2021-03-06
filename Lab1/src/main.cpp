#include <iostream>
#include <fstream>
#include <algorithm>
#include "corner_stitching.hpp"
using std::cout;
using std::endl;
using std::cerr;

int main(int argc, char ** argv){
    std::string out_name;
    if(argc == 2){
        out_name = "output.txt";
    }else if(argc ==3){
        out_name = argv[2];
    }else{
        cerr << "Error: wrong parameters" << endl;
        exit(1);
    }
    std::ifstream fin{argv[1]};
    if(!fin){
        cerr << "Error: can not open file "<< argv[1] << endl;
        exit(1);
    }

    std::ofstream fout{out_name};
    if(!fout){
        cerr << "Error: can not open file "<< out_name << endl;
        exit(1);
    }

    int width, height, x, y;
    std::vector<Tile*>tiles;
    std::vector<std::vector<int>>points;
    fin >> width >> height;
    CornerStitching cs(width, height);
    std::string temp;
    while(fin>>temp){
        if(temp=="P"){
            fin >> x >> y;
            Tile* t = cs.findPoint(x,y);
            points.push_back({t->x,t->y});
        }else{
            fin >> x >> y >> width >> height;
            Tile *t = cs.insertTile(x,y,width,height,std::stoi(temp));
            tiles.push_back(t);
        }
    }
    
    std::sort(tiles.begin(),tiles.end(),[](Tile* lhs, Tile* rhs){return lhs->index < rhs->index;});
    cout<<"result:"<<endl;
    cout<<cs.getTileNum()<<endl;
    fout<<cs.getTileNum()<<endl;

    for(Tile *t: tiles){
        int space_num, block_num;
        cs.countNeighbor(t,space_num,block_num);
        cs.countNeighbor(t,space_num,block_num);
        cout << t->index << " " << block_num <<" "<<space_num<<endl;
        fout << t->index << " " << block_num <<" "<<space_num<<endl;
    }
    for(std::vector<int>& p: points){
        cout << p[0] << " " << p[1] <<endl;
        fout << p[0] << " " << p[1] <<endl;
    }
    return 0;
}
