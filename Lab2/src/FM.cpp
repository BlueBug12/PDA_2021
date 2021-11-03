#include "FM.hpp"

FM::FM(const std::string & file_name, float ratio){
    best_record = NULL;
    readInput(file_name);
    if(ratio>=0.5){
        max_group = cell_num*ratio;
    }else{
        max_group = cell_num*(1-ratio);
    }
    min_group = cell_num - max_group;
    _shift = 0;
    for(auto & pair: all_cells){
        _shift = std::max(_shift,pair.second->nets.size());
    }
    _l_group.resize(2*_shift + 1);
    _r_group.resize(2*_shift + 1);
#ifdef DEBUG
    std::cout<<"min group:"<<min_group<<std::endl;
    std::cout<<"max group:"<<max_group<<std::endl;
    std::cout<<"shift value:"<<_shift<<std::endl;
#endif
}
FM::~FM(){
    for(Net *n: all_nets)
        delete n;
    for(auto pair: all_cells)
        delete pair.second;
    delete best_record;
}
void FM::readInput(const std::string & file_name){
    std::ifstream fin{file_name};
    if(!fin){
        std::cerr << "Error: can not open file "<< file_name << std::endl;
        exit(1);
    }
    fin >> net_num >> cell_num;
    std::string line;
    std::getline(fin,line);
    int net_id = 0;
    bool side = true;
    while(std::getline(fin,line)){
        if(line.back()!=' ')
            line.push_back(' ');
        size_t pos = 0;
        size_t pre_pos = 0;
        std::set<Cell*>cells;
        Net *n = new Net(net_id);
        while((pos = line.find(" ",pre_pos))!=std::string::npos){
            std::string substr = line.substr(pre_pos,pos-pre_pos);
            int cell_id = std::stoi(substr);
            Cell* c;
            auto cell_iter = all_cells.find(cell_id);
            if(cell_iter==all_cells.end()){
                c = new Cell(cell_id,side);
                all_cells[cell_id] = c;
                side = !side;
            }else{
                c = cell_iter->second;
            }
            cells.insert(c);
            c->nets.insert(net_id);
            if(c->left){
                n->l_cells++;
                _left_num++;
            }
            else{
                n->r_cells++;
                _right_num++;
            }
            
            pre_pos = pos+1;
            
        }
        net_id++;
        n->cells = std::move(cells);
        all_nets.push_back(n);
    }
    fin.close();
#ifdef DEBUG
    if(all_nets.size()!=net_num){
        throw std::runtime_error("Error: net number unmatched");
    }
    if(all_cells.size()!=cell_num){
        throw std::runtime_error("Error: cell number unmatched");
    }
    for(Net *n : all_nets){
        std::cout<<"Net "<<n->net_id<<" :";
        for(Cell* c: n->cells){
            std::cout<<c->cell_id<<" ";
        }
        std::cout<<std::endl;
        std::cout<<"left number:"<<n->l_cells<<std::endl;
        std::cout<<"right number:"<<n->r_cells<<std::endl;
        std::cout<<std::endl;
    }

    for(auto pair: all_cells){
        Cell* c = pair.second;
        std::cout<<"Cell "<<c->cell_id<<" :";
        for(int i : c->nets){
            std::cout<<i<<" ";
        }
        std::cout<<std::endl;
        if(c->left)
            std::cout<<"left"<<std::endl;
        else
            std::cout<<"right"<<std::endl;
        std::cout<<std::endl;
    }    
    
#endif
}

void FM::writeOutput(const std::string & file_name){
    std::ofstream fout{file_name};
    if(!fout){
        std::cerr << "Error: can not open file "<< file_name << std::endl;
        exit(1);
    }
    std::vector<std::pair<int,Cell*>>v;//first: index, second: Cell ptr
    for(auto pair: all_cells)
        v.push_back({pair.second->cell_id,pair.second});

    sort(v.begin(),v.end());

    for(size_t i =0;i<v.size();++i){
        fout << v[i].second->left <<std::endl;
    }
    fout.close();
}
void FM::initialGain(){
    for(size_t i=0;i<all_nets.size();++i){
        Net *n = all_nets[i];
        if(n->l_cells == 0){
            for(Cell* c: n->cells){
                c->gain--;
#ifdef DEBUG
                if(!c->left){
                    throw std::runtime_error("Error: cell in the wrong group.");
                }
#endif
            }
        }       
    }
}
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
