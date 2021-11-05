#include "FM.hpp"

FM::FM(const std::string & file_name, float ratio){
    _left_num = 0;
    _right_num = 0;
    stop = false;
    readInput(file_name);
    min_cut = getCutSize();
    best_balance = std::min((float)_left_num/cell_num,(float)_right_num/cell_num);

    if(ratio>=0.5){
        max_group = cell_num*ratio;
    }else{
        max_group = cell_num*(1-ratio);
    }
    min_group = cell_num - max_group;
    if(min_group>max_group)
        std::swap(min_group,max_group);
    _shift = 0;
    for(auto & pair: all_cells){
        _shift = std::max(_shift,(int)pair.second->nets.size());
    }
    _l_group.resize(2*_shift + 1);
    _r_group.resize(2*_shift + 1);
#ifdef PRINTER
    std::cout<<std::endl; 
    std::cout<<"min group:"<<min_group<<std::endl;
    std::cout<<"max group:"<<max_group<<std::endl;
    std::cout<<"shift value:"<<_shift<<std::endl;
    std::cout<<std::endl; 

#endif
}
FM::~FM(){
    for(Net *n: all_nets)
        delete n;
    for(auto pair: all_cells)
        delete pair.second;
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
                if(c->left)
                    _left_num++;
                else
                    _right_num++;
            }else{
                c = cell_iter->second;
            }

            n->cells.push_back(c);
            c->nets.push_back(n);

            if(c->left){
                n->init_l_cells++;
            }
            else{
                n->init_r_cells++;
            }
            
            pre_pos = pos+1;
            
        }
        net_id++;
        all_nets.push_back(n);
        n->l_cells = n->init_l_cells;
        n->r_cells = n->init_r_cells;
    }
    
    for(auto pair: all_cells)
        ordered_cells.push_back({pair.second->cell_id,pair.second});
    sort(ordered_cells.begin(),ordered_cells.end());

    fin.close();
#ifdef DEBUG
    if(all_nets.size()!=net_num){
        throw std::runtime_error("Error: net number unmatched");
    }
    if(all_cells.size()!=cell_num){
        throw std::runtime_error("Error: cell number unmatched");
    }
#endif
}

void FM::writeOutput(const std::string & file_name){
    std::ofstream fout{file_name};
    if(!fout){
        std::cerr << "Error: can not open file "<< file_name << std::endl;
        exit(1);
    }

    for(size_t i =0;i<ordered_cells.size();++i){
        fout << ordered_cells[i].second->ret_left <<std::endl;
    }
    fout.close();
}

void FM::resetGain(){
    for(auto pair: all_cells)
        pair.second->gain = 0;
    for(size_t i=0;i<all_nets.size();++i){
        Net *n = all_nets[i];
        n->l_cells = n->init_l_cells;
        n->r_cells = n->init_r_cells;

#ifdef DEBUG
            if((int)n->cells.size()!=n->l_cells+n->r_cells)
                throw std::runtime_error("Error: cell size unmatched(7).");
            int l_c = 0;
            int r_c = 0;
            for(Cell* c: n->cells){
                if(c->init_left)
                    ++l_c;
                else
                    ++r_c;
            }
            if(l_c!=n->l_cells)
                throw std::runtime_error("Error: cell size unmatched(8).");

            if(r_c!=n->r_cells)
                throw std::runtime_error("Error: cell size unmatched(9).");
#endif
        //all cells locate at right / left group
        if(n->l_cells == 0){
            for(Cell *c: n->cells){
                c->gain--;
#ifdef DEBUG
                if(c->init_left){
                    throw std::runtime_error("Error: cell in the wrong group(1).");
                }
#endif
            }
        }else if(n->r_cells == 0){
            for(Cell* c: n->cells){
                c->gain--;
#ifdef DEBUG
                if(!c->init_left){
                    throw std::runtime_error("Error: cell in the wrong group(2).");
                }
#endif
            }
        }else{//at least one cell locate at both sides
            if(n->l_cells == 1){
                Cell *c = findTarget(n->net_id, true);//left side == true
                c->gain++;
            }
            if(n->r_cells == 1){
                Cell *c = findTarget(n->net_id, false);//left side == true
                c->gain++;
            }
        }       
    }
}

void FM::initialize(){
    resetGain();
    std::cout<<"Best cut size:"<<min_cut<<std::endl;
    std::cout<<"cut size:"<<getCutSize()<<std::endl;

    //set the initial bucket list
    for(size_t i=0;i<ordered_cells.size();++i){
        Cell *c = ordered_cells.at(i).second;
        int index = c->gain+_shift;
#ifdef DEBUG
        if(index<0 || index>=(int)_l_group.size()){
            throw std::runtime_error("Error: wrong shifted index.");
        }
#endif
        if(c->left){
            _l_group.at(index).push_back(c);
        }else{
            _r_group.at(index).push_back(c);
        }
    }
}
void FM::updateGain(Cell* target){
    //in this function, we don't care whether or not the cell is lock
    target->lock = true;
    int origin_gain = target->gain;
    std::unordered_map<Cell *, int> table;//record the origin gain of modified cell
    if(target->left){//move cell from left to right
        _left_num--;
        _right_num++;
        target->left = false;
        for(Net* n: target->nets){
            if(n->r_cells == 0){//To
                for(Cell* c: n->cells){
                    if(c->lock)
                        continue;
                    if(table.find(c)==table.end()){
                        table.insert({c,c->gain});
                    }
                    c->gain++;
    #ifdef DEBUG
                    if(!c->left){
                        std::cerr<<"found cell "<<c->cell_id<<std::endl;
                        throw std::runtime_error("Error: cell in the wrong group(3).");
                    }
    #endif
                }
            }else if(n->r_cells == 1){
                Cell *c = findTarget(n->net_id,false);
                if(c){
                    if(table.find(c)==table.end()){
                        table.insert({c,c->gain});
                    }
                    c->gain--;
                }
            }

            n->l_cells--;
            n->r_cells++;

            if(n->l_cells == 0){//From
                for(Cell* c: n->cells){
                    if(c->lock)
                        continue;
                    if(table.find(c)==table.end()){
                        table.insert({c,c->gain});
                    }
                    c->gain--;
                }
            }else if(n->l_cells == 1){
                Cell* c = findTarget(n->net_id,true);
                if(c){
                    if(table.find(c)==table.end()){
                        table.insert({c,c->gain});
                    }
                    c->gain++;
                }
            }
#ifdef DEBUG
            if((int)n->cells.size()!=n->l_cells+n->r_cells)
                throw std::runtime_error("Error: cell size unmatched(4).");
            int l_c = 0;
            int r_c = 0;
            for(Cell* c: n->cells){
                if(c->left)
                    ++l_c;
                else
                    ++r_c;
            }
            if(l_c!=n->l_cells)
                throw std::runtime_error("Error: cell size unmatched(5).");

            if(r_c!=n->r_cells)
                throw std::runtime_error("Error: cell size unmatched(6).");
#endif
        }
    }else{//move cell from right to left
        _right_num--;
        _left_num++;
        target->left = true;
        for(Net *n: target->nets){
            if(n->l_cells == 0){//To
                for(Cell* c: n->cells){
                    if(c->lock)
                        continue;
                    if(table.find(c)==table.end()){
                        table.insert({c,c->gain});
                    }
                    c->gain++;
    #ifdef DEBUG
                    if(c->left){
                        throw std::runtime_error("Error: cell in the wrong group(4).");
                    }
    #endif
                }
            }else if(n->l_cells == 1){
                Cell *c = findTarget(n->net_id,true);
                if(c){
                    if(table.find(c)==table.end()){
                       table.insert({c,c->gain});
                    }
                    c->gain--;
                }
            }

            n->r_cells--;
            n->l_cells++;


            if(n->r_cells == 0){//from
                for(Cell* c: n->cells){
                    if(c->lock)
                        continue;
                    if(table.find(c)==table.end()){
                        table.insert({c,c->gain});
                    }
                    c->gain--;
                }
            }else if(n->r_cells == 1){
                Cell *c = findTarget(n->net_id,false);
                if(c){
                    if(table.find(c)==table.end()){
                        table.insert({c,c->gain});
                    }
                    c->gain++;
                }
            }
         
#ifdef DEBUG
            if((int)n->cells.size()!=n->l_cells+n->r_cells)
                throw std::runtime_error("Error: cell size unmatched(1).");
            int l_c = 0;
            int r_c = 0;
            for(Cell* c: n->cells){
                if(c->left)
                    ++l_c;
                else
                    ++r_c;
            }
            if(l_c!=n->l_cells)
                throw std::runtime_error("Error: cell size unmatched(2).");

            if(r_c!=n->r_cells)
                throw std::runtime_error("Error: cell size unmatched(3).");
#endif
        }
    }

    //update bucket list
    for(auto pair: table){
        Cell *c = pair.first;
        int index = pair.second + _shift;
        std::vector<std::list<Cell*>> *group;
        if(c->left){
            group = &_l_group;
        }else{
            group = &_r_group;
        }
        std::list<Cell*>& bucket = group->at(index);

        for(auto iter = bucket.begin();iter!=bucket.end();++iter){
            if(*iter == c){
                bucket.erase(iter);
#ifdef DEBUG
                if(c->gain + _shift>=(int)group->size()||c->gain + _shift<0)
                    throw std::runtime_error("Error: index out of range when update bucket.");
#endif
                group->at(c->gain + _shift).push_back(c);
                break;
            }
        }
    }
    int pre_sum = 0;
    if(!recorder.empty())
        pre_sum = recorder.back().gain_sum;
    Record r(target,
            origin_gain,
            origin_gain+pre_sum,
            getCutSize(),
            std::min((float)_left_num/cell_num,(float)_right_num/cell_num));
    recorder.push_back(std::move(r));
}
Cell* FM::chooseCell(){
    bool move_left = true;
    bool move_right = true;
    if(_left_num - 1 < min_group)
        move_right = false;
    if(_right_num - 1 < min_group)
        move_left = false;
    //find the unlock cell with greatest gain without viloating balance
    for(int i = (int)_l_group.size()-1;i>=0;--i){
        if(move_right && !_l_group.at(i).empty()){
            for(auto iter = _l_group.at(i).begin();iter!=_l_group.at(i).end();++iter){
                if(!(*iter)->lock){
                    Cell *c = (*iter);
                    _l_group.at(i).erase(iter);
                    return c;
                }
            }
        }
        if(move_left && !_r_group.at(i).empty()){
            for(auto iter = _r_group.at(i).begin();iter!=_r_group.at(i).end();++iter){
                if(!(*iter)->lock){
                    Cell *c = (*iter);
                    _r_group.at(i).erase(iter);
                    return c;
                }
            }
        }
    }
#ifdef DEBUG
    size_t l = 0;
    for(auto pair: all_cells){
        Cell *c = pair.second;
        if(c->lock)
            ++l;
    }
    std::cerr<<"lock: "<<l<<"  unlock: "<<all_nets.size()-l<<std::endl;
    std::cerr<<"move right "<<move_right<<std::endl;
    std::cerr<<"move lefti "<<move_left<<std::endl;
    throw std::runtime_error("Error: can not find unlock cell in the list.");
#endif
    return NULL;
} 
inline bool FM::checkBalance(Cell* c){
    if(c->left && _left_num - 1 < min_group){
        return false;
    }
    if(!c->left && _right_num - 1 < min_group){
        return false;
    }
    return true;
}
void FM::unlockAll(){
    _left_num = 0;
    _right_num = 0;
    for(size_t i=0;i<ordered_cells.size();++i){
        Cell *c = ordered_cells.at(i).second;
        c->left = c->init_left;
    #ifdef DEBUG
        if(!c->lock)
            throw std::runtime_error("Error: exist unlock cell after one pass.");
    #endif
        c->lock = false;
        if(c->left)
            ++_left_num;
        else
            ++_right_num;
    }
}

Cell* FM::findTarget(const int net_id, const bool left){
    std::vector<Cell*>& c_set = all_nets.at(net_id)->cells;
    for(Cell* c: c_set){
        if(c->left == left && !c->lock){
            return c;
        }
    }
    return NULL;
}

void FM::storeResult(){
    stop = true;
    int min_cut_local = INT_MAX;
    float best_balance_local = 1.0;
    int index = -1;
    for(size_t i=0;i<recorder.size();++i){
        Record & r = recorder.at(i);
        if(min_cut_local > r.cut_size || (min_cut_local == r.cut_size && r.balance_ratio > best_balance_local)){
            index = (int)i;
            best_balance_local = r.balance_ratio;
            min_cut_local = r.cut_size;
        }
        if(r.gain_sum>0)//if there are some improvements, keep iteration
            stop = false;
    }
    if(stop)
        return;
    
    //reset the solution 
    bool flag = false;
    if(min_cut > min_cut_local || (min_cut == min_cut_local && best_balance_local > best_balance)){
        flag = true;
        min_cut = min_cut_local;
        best_balance = best_balance_local;
    }
        flag = true;
#ifdef PRINTER
    std::cout<<"Result in this round:"<<std::endl;
    std::cout<<"min cut size:"<<min_cut_local<<std::endl;
    std::cout<<"best balance ratio:"<<best_balance_local<<std::endl;
    std::cout<<"index:"<<index<<std::endl;
    if(flag)
        std::cout<<"Find better solution."<<std::endl;
    else
        std::cout<<"No imporement."<<std::endl;
    std::cout<<std::endl;
#endif
    for(int i = 0;i<=index;++i){
       Record & r = recorder.at(i);
       Cell* c = r.moved_cell;
       c->init_left = !c->init_left;
       if(flag)
           c->ret_left = c->init_left;
       for(Net *n: c->nets){
           if(c->init_left){
               n->init_r_cells--; 
               n->init_l_cells++;
           }else{
               n->init_r_cells++; 
               n->init_l_cells--;
           }
       }
    }
    recorder.clear();
}

inline int FM::getCutSize(){
    int cut_size = 0;
    for(Net *n:all_nets){
        if(n->l_cells !=0 && n->r_cells !=0){//exist cross edge
            ++cut_size;
        }
    }
    return cut_size;
}
