#include "FM.hpp"

FM::FM(const std::string & file_name, float ratio){
    cell_counter[0] = cell_counter[1] = 0;
    readInput(file_name);
    max_group = cell_num*ratio;
    min_group = cell_num - max_group;
    if(min_group>max_group)
        std::swap(min_group,max_group);

    current_cut = min_cut = getCutSize();
    answer = group;
}

void FM::readInput(const std::string & file_name){
    std::ifstream fin{file_name};
    if(!fin){
        std::cerr << "Error: can not open file "<< file_name << std::endl;
        exit(1);
    }
    fin >> net_num >> cell_num;

    net_list.resize(net_num);
    cell_num_in_net[0].resize(net_num,0);
    cell_num_in_net[1].resize(net_num,0);
    cell_list.resize(cell_num);
    group.resize(cell_num,false);
    cell_gain.resize(cell_num,0);
    
    std::string line;
    bool side = true;
    int c_id;
    std::getline(fin,line);
    for(size_t i=0;i<net_num;++i){
        std::getline(fin,line);
        std::istringstream ss(line);
        size_t pseudo_id;
        bool cur_group;
        while(ss >> c_id){
            if(key.find(c_id)==key.end()){
                pseudo_id = key.size();
                key[c_id] = pseudo_id;
                group[pseudo_id] = side;
                cur_group = side;
                side = !side;
                cell_counter[cur_group]++;

            }else{
                pseudo_id = key[c_id];
                cur_group = group[pseudo_id];
            }
            cell_num_in_net[cur_group][i]++;
            net_list[i].push_back(pseudo_id);
            cell_list[pseudo_id].push_back(i);
        }
        if(net_list[i].size()==1){
            cell_num_in_net[cur_group][i]--;
            //cell_counter[cur_group]--;
            net_list[i].clear();
            cell_list[pseudo_id].pop_back();
            
            --i;
            --net_num;
        }
    }
    assert(cell_counter[0]+cell_counter[1]==(int)cell_num);
    fin.close();
}

void FM::writeOutput(const std::string & file_name){
    std::ofstream fout{file_name};
    if(!fout){
        std::cerr << "Error: can not open file "<< file_name << std::endl;
        exit(1);
    }
    for(auto & p:key){
        fout << answer[p.second] << std::endl;
    }
    fout.close();
}
int FM::calGain(size_t cell_id){
    int gain = 0;
    for(int net_id:cell_list[cell_id]){
        if(cell_num_in_net[group[cell_id]][net_id] == 1)//cell  is the only one locate at this side for this net
            gain++;
        else if(cell_num_in_net[!group[cell_id]][net_id] == 0)//the other side doesn't exist any cell for this net
            gain--;
    }
    return gain;
}

void FM::initialize(){
    for(size_t i=0;i<cell_num;++i){
        int gain = calGain(i);
        bucket[group[i]].insert({gain,i});
        cell_gain[i] = gain;
    }
}

void FM::updateGain(size_t cell_id){
    current_cut -= cell_gain[cell_id];
    bool from = group[cell_id];
    bool to = !group[cell_id];
    cell_counter[from]--;
    cell_counter[to]++;
    assert(cell_counter[from]>=(int)min_group); 
    for(size_t net_id: cell_list[cell_id]){
        if(cell_num_in_net[to][net_id] == 0){
            for(size_t con_cell:net_list[net_id]){
                auto it = bucket[from].find({cell_gain[con_cell],con_cell});
                if(it!= bucket[from].end()){
                    bucket[from].erase(it);
                    cell_gain[con_cell]++;
                    bucket[from].insert({cell_gain[con_cell],con_cell});
                }
            }
        }else if(cell_num_in_net[to][net_id] == 1){
            for(size_t con_cell:net_list[net_id]){
                if(group[con_cell]!=to)
                    continue;
                auto it = bucket[to].find({cell_gain[con_cell],con_cell});
                if(it!= bucket[to].end()){
                    bucket[to].erase(it);
                    cell_gain[con_cell]--;
                    bucket[to].insert({cell_gain[con_cell],con_cell});
                }
            }
        }

        cell_num_in_net[to][net_id]++;
        cell_num_in_net[from][net_id]--;

        if(cell_num_in_net[from][net_id]==0){
            for(size_t con_cell:net_list[net_id]){
                auto it = bucket[to].find({cell_gain[con_cell],con_cell});
                if(it!=bucket[to].end()){
                    bucket[to].erase(it);
                    cell_gain[con_cell]--;
                    bucket[to].insert({cell_gain[con_cell],con_cell});
                }
            }
        }else if(cell_num_in_net[from][net_id]==1){
            for(size_t con_cell:net_list[net_id]){
                if(group[con_cell]!=from)
                    continue;
                auto it = bucket[from].find({cell_gain[con_cell],con_cell});
                if(it!=bucket[from].end()){
                    bucket[from].erase(it);
                    cell_gain[con_cell]++;
                    bucket[from].insert({cell_gain[con_cell],con_cell});
                }
            }
        }
    }
    
    group[cell_id] = !group[cell_id];
#ifdef DEBUG
    if(getCutSize()!=current_cut){
        std::cerr<<"wrong!"<<std::endl;
        exit(1);
    }
#endif 
    if(current_cut<min_cut){
        answer.clear();
        answer = group;
        min_cut = current_cut;
    }
}

size_t FM::chooseCell(){
    size_t cell_id;
    if(bucket[0].empty()||cell_counter[0]==(int)min_group){
        cell_id = bucket[1].begin()->second;
        assert(group[cell_id]==true);
        bucket[1].erase(bucket[1].begin());
    }else if(bucket[1].empty()||cell_counter[1]==(int)min_group){
        cell_id = bucket[0].begin()->second;
        bucket[0].erase(bucket[0].begin());
        assert(group[cell_id]==false);
    }else{
        if(bucket[0].begin()->second>=bucket[1].begin()->second){
            cell_id = bucket[0].begin()->second;
            bucket[0].erase(bucket[0].begin());
            assert(group[cell_id]==false);
        }else{
            cell_id = bucket[1].begin()->second;
            bucket[1].erase(bucket[1].begin());
            assert(group[cell_id]==true);
        }
    }
    return cell_id;
} 

inline int FM::getCutSize(){
    int cut_size = 0;
    for(size_t i=0;i<net_num;++i){
#ifdef DEBUG
        assert(cell_num_in_net[0][i]>=0);
        assert(cell_num_in_net[1][i]>=0);
#endif
        if(cell_num_in_net[0][i] && cell_num_in_net[1][i])
            ++cut_size;
    }
    return cut_size;
}

void FM::undoGroup(){
    for(size_t i=0;i<cell_num;++i){
        if(group[i]!=answer[i]){
            if(group[i]){
                cell_counter[1]--;
                cell_counter[0]++;
                for(size_t net_id:cell_list[i]){
                    cell_num_in_net[1][net_id]--;
                    cell_num_in_net[0][net_id]++;
                }
            }else{
                cell_counter[1]++;
                cell_counter[0]--;
                for(size_t net_id:cell_list[i]){
                    cell_num_in_net[1][net_id]++;
                    cell_num_in_net[0][net_id]--;
                }
            }
            group[i] = answer[i];
        }
    }
}

void FM::run(){
    std::cout<<"Initial cutsize:"<<current_cut<<std::endl;
    int origin_min_cut = 1e9;
    int counter = 1;
    while(1){
        origin_min_cut = min_cut;
        initialize();
        for(size_t i=0;i<cell_num;++i){
            size_t cell_id = chooseCell();
            updateGain(cell_id);
        }
        if(origin_min_cut == min_cut)
            break;
#ifdef DEBUG
        assert(bucket[0].empty() && bucket[1].empty());            
#endif 
        undoGroup();

#ifdef DEBUG
        std::vector<int>test[2];
        test[0].resize(net_num);
        test[1].resize(net_num);
        for(size_t i=0;i<cell_num;++i){
            for(size_t net_id:cell_list[i]){
                test[group[i]][net_id]++;
            }
        }

        for(size_t i=0;i<net_num;++i){
            assert(test[0][i]==cell_num_in_net[0][i]&&test[1][i]==cell_num_in_net[1][i]);
        }
        
        assert(getCutSize()==min_cut);
#endif
        current_cut = min_cut;
        std::cout<<"Round "<<counter++<<" cutsize: "<<min_cut<<std::endl;
    }
    std::cout<<"final cutsize:"<<min_cut<<std::endl;
}
