#include "FM.hpp"

FM::FM(const std::string & file_name, float ratio){
    stop = false;
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
        _shift = std::max(_shift,(int)pair.second->nets.size());
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

    for(size_t i =0;i<ordered_cells.size();++i){
        fout << ordered_cells[i].second->ret_left <<std::endl;
    }
    fout.close();
}

void FM::initialGain(){
    for(size_t i=0;i<all_nets.size();++i){
        Net *n = all_nets[i];
        //all cells locate at right / left group
        if(n->l_cells == 0){
            for(Cell *c: n->cells){
                c->gain--;
#ifdef DEBUG
                if(c->left){
                    throw std::runtime_error("Error: cell in the wrong group(1).");
                }
#endif
            }
        }else if(n->r_cells == 0){
            for(Cell* c: n->cells){
                c->gain--;
#ifdef DEBUG
                if(!c->left){
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
    best_record = new Record(getCutSize(),std::min((float)_left_num/cell_num,(float)_right_num/cell_num));
    recorder.push_back(*best_record);

    //set the initial bucket list
    for(size_t i=0;i<ordered_cells.size();++i){
        Cell *c = ordered_cells.at(i).second;
        int index = c->gain+_shift;
#ifdef DEBUG
        if(index<0 || index>=(int)ordered_cells.size()){
            throw std::runtime_error("Error: wrong shifted index.");
        }
#endif
        if(c->left){
            _l_group.at(index).push_back(c);
        }else{
            _r_group.at(index).push_back(c);
        }
    }

#ifdef DEBUG
    std::cout<<"After gain initialization:"<<std::endl;
    for(auto pair: all_cells){
        int index = pair.first;
        Cell *c = pair.second;
        std::cout<<"Initial gain of Cell "<<index<<":"<<c->gain<<std::endl;
    }

    std::cout<<"Initial bucket list:"<<std::endl;
    std::cout<<"Left list:"<<std::endl;
    for(int i=0;i<(int)_l_group.size();++i){
        std::cout<<i-_shift<<"->";
        for(auto it = _l_group.at(i).begin();it!=_l_group.at(i).end();++it){
            std::cout<<(*it)->cell_id<<" ";
        }
        std::cout<<std::endl;
    }
    std::cout<<"Right list:"<<std::endl;
    for(int i=0;i<(int)_r_group.size();++i){
        std::cout<<i-_shift<<"->";
        for(auto it = _r_group.at(i).begin();it!=_r_group.at(i).end();++it){
            std::cout<<(*it)->cell_id<<" ";
        }
        std::cout<<std::endl;
    }
#endif 
}
void FM::updateGain(Cell* target){
    //in this function, we don't care whether or not the cell is lock
    target->lock = true;
    int origin_gain = target->gain;
    std::unordered_map<Cell *, int> table;//record the origin gain of modified cell
    if(target->left){//move cell from left to right
        _left_num--;
        _right_num++;
        for(size_t i = 0;i<all_nets.size();++i){
            Net *n = all_nets.at(i);
            if(n->r_cells == 0){//To
                for(Cell* c: n->cells){
                    if(table.find(c)==table.end()){
                        table.insert({c,c->gain});
                    }
                    c->gain++;
    #ifdef DEBUG
                    if(!c->left){
                        throw std::runtime_error("Error: cell in the wrong group(3).");
                    }
    #endif
                }
            }else if(n->r_cells == 1){
                Cell *c = findTarget(n->net_id,false);
                if(table.find(c)==table.end()){
                    table.insert({c,c->gain});
                }
                c->gain--;
            }

            n->l_cells--;
            _left_num--;
            n->r_cells++;
            _right_num++;

            target->left = false;

            if(n->l_cells == 0){
                for(Cell* c: n->cells){
                    if(table.find(c)==table.end()){
                        table.insert({c,c->gain});
                    }
                    c->gain--;
                }
            }else if(n->l_cells == 1){
                Cell* c = findTarget(n->net_id,true);
                if(table.find(c)==table.end()){
                    table.insert({c,c->gain});
                }
                c->gain++;
            }
        }
    }else{//move cell from right to left
        _left_num++;
        _right_num--;
        for(size_t i = 0;i<all_nets.size();++i){
            Net *n = all_nets.at(i);
            if(n->l_cells == 0){//To
                for(Cell* c: n->cells){
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
                if(table.find(c)==table.end()){
                   table.insert({c,c->gain});
                }
                c->gain--;
            }

            n->r_cells--;
            _right_num--;
            n->l_cells++;
            _left_num++;

            target->left = true;

            if(n->r_cells == 0){
                for(Cell* c: n->cells){
                    if(table.find(c)==table.end()){
                        table.insert({c,c->gain});
                    }
                    c->gain--;
                }
            }else if(n->r_cells == 1){
                Cell *c = findTarget(n->net_id,false);
                if(table.find(c)==table.end()){
                    table.insert({c,c->gain});
                }
                c->gain++;
            }
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
                group->at(c->gain + _shift).push_back(c);
                break;
            }
        }
    }
    Record r(target,
            origin_gain,
            origin_gain+recorder.back().gain_sum,
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
    throw std::runtime_error("Error: can not find unlock cell in the list.");
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
    for(size_t i=0;i<ordered_cells.size();++i){
        Cell *c = ordered_cells.at(i).second;
    #ifdef DEBUG
        if(!c->lock)
            throw std::runtime_error("Error: exist unlock cell after one pass.");
    #endif
        c->lock = false;
        c->init_left = c->left;
    }
}

Cell* FM::findTarget(const int net_id, const bool left){
    std::set<Cell*>& c_set = all_nets.at(net_id)->cells;
    for(Cell* c: c_set){
        if(c->left == left){
            return c;
        }
    }
    throw std::runtime_error("Error: can not find target cell in net.");
}

void FM::storeResult(){
    stop = true;
    float min_cut = best_record->cut_size;
    float best_balance = best_record->balance_ratio;
    int index = -1;
    for(size_t i=0;i<recorder.size();++i){
        Record & r = recorder.at(i);
        if(min_cut > r.cut_size || (min_cut == r.cut_size && r.balance_ratio > best_balance)){
            index = (int)i;
            best_balance = r.balance_ratio;
            min_cut = r.cut_size;
        }
        if(r.gain_sum>0)
            stop = false;
    }
    if(index == -1)
        return;

    //reset the solution 
    for(int i=0;i<=index;++i){
       Record & r = recorder.at(i);
       Cell* c = r.moved_cell;
       c->ret_left = !c->init_left;
    }
    delete best_record;
    *best_record = recorder.at(index);
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
