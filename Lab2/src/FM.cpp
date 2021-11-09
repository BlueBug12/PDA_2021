#include "FM.hpp"

FM::FM(const std::string & file_name, float ratio){
    cell_counter[0] = cell_counter[1] = 0;
    stop = false;
    readInput(file_name);
    if(ratio>=0.5){
        max_group = cell_num*ratio;
    }else{
        max_group = cell_num*(1-ratio);
    }
    min_group = cell_num - max_group;
    if(min_group>max_group)
        std::swap(min_group,max_group);

    min_cut = getCutSize();
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
        while(ss >> c_id){
            size_t pseudo_id;
            bool cur_group;
            if(key.find(c_id)==key.end()){
                pseudo_id = key.size();
                key[c_id] = pseudo_id;
                group.at(pseudo_id) = side;
                cur_group = side;
                side = !side;
            }else{
                pseudo_id = key[c_id];
                cur_group = group.at(pseudo_id);
            }
            cell_num_in_net[cur_group].at(i)++;
            cell_counter[cur_group]++;
            net_list.at(i).push_back(pseudo_id);
            cell_list.at(pseudo_id).push_back(i);
        }
    }
    fin.close();
}

void FM::writeOutput(const std::string & file_name){
    std::ofstream fout{file_name};
    if(!fout){
        std::cerr << "Error: can not open file "<< file_name << std::endl;
        exit(1);
    }
    for(auto & p:key){
        fout << answer.at(p.second) << std::endl;
    }
    fout.close();
}
int FM::calGain(size_t cell_id){
    int gain = 0;
    for(int net_id:cell_list.at(cell_id)){
        if(cell_num_in_net[group[cell_id]].at(net_id) == 1)//cell  is the only one locate at this side for this net
            gain++;
        else if(cell_num_in_net[!group[cell_id]].at(net_id) == 0)//the other side doesn't exist any cell for this net
            gain--;
    }
    return gain;
}


void FM::initialize(){
    for(size_t i=0;i<cell_num;++i){
        int gain = calGain(i);
        bucket[group.at(i)].insert({gain,i});
        cell_gain.at(i) = gain;
    }
}


void FM::updateGain(size_t cell_id){
    bool from = group.at(cell_id);
    bool to = !group.at(cell_id);
    cell_counter[from]--;
    cell_counter[to]++;
    
    for(size_t net_id: cell_list.at(cell_id)){
        if(cell_num_in_net[to].at(net_id) == 0){
            for(size_t con_cell:net_list.at(net_id)){
                auto it = bucket[from].find({cell_gain.at(con_cell),con_cell});
                if(it!= bucket[from].end()){
                    bucket[from].erase(it);
                    cell_gain.at(con_cell)++;
                    bucket[from].insert({cell_gain.at(con_cell),con_cell});
                }
            }
        }else if(cell_num_in_net[to].at(net_id) == 1){
            for(size_t con_cell:net_list.at(net_id)){
                if(group.at(con_cell)!=to)
                    continue;
                auto it = bucket[to].find({cell_gain.at(con_cell),con_cell});
                if(it!= bucket[to].end()){
                    bucket[to].erase(it);
                    cell_gain.at(con_cell)--;
                    bucket[to].insert({cell_gain.at(con_cell),con_cell});
                }
            }
        }
        cell_num_in_net[to].at(net_id)++;
        cell_num_in_net[from].at(net_id)--;
        if(cell_num_in_net[from].at(net_id)==0){
            for(size_t con_cell:net_list.at(net_id)){
                auto it = bucket[to].find({cell_gain.at(con_cell),con_cell});
                if(it!=bucket[to].end()){
                    bucket[to].erase(it);
                    cell_gain.at(con_cell)--;
                    bucket[to].insert({cell_gain.at(con_cell),con_cell});
                }
            }
        }else if(cell_num_in_net[from].at(net_id)==1){
            for(size_t con_cell:net_list.at(net_id)){
                if(group.at(con_cell)!=from)
                    continue;
                auto it = bucket[from].find({cell_gain.at(con_cell),con_cell});
                if(it!=bucket[from].end()){
                    bucket[from].erase(it);
                    cell_gain.at(con_cell)++;
                    bucket[from].insert({cell_gain.at(con_cell),con_cell});
                }
            }
        }

    }
    
    group.at(cell_id) = !group.at(cell_id);
    if(getCutSize()<min_cut){
        min_cut = getCutSize();
        answer.clear();
        answer = group;
    }
}

size_t FM::chooseCell(){
    size_t cell_id;
    if(bucket[0].empty()||cell_counter[0]==(int)min_group){
        cell_id = bucket[1].begin()->second;
        bucket[1].erase(bucket[1].begin());
    }else if(bucket[1].empty()||cell_counter[1]==(int)min_group){
        cell_id = bucket[0].begin()->second;
        bucket[0].erase(bucket[0].begin());
    }else{
        if(bucket[0].begin()->second>=bucket[1].begin()->second){
            cell_id = bucket[0].begin()->second;
            bucket[0].erase(bucket[0].begin());
        }else{
            cell_id = bucket[1].begin()->second;
            bucket[1].erase(bucket[1].begin());
        }
    }
    return cell_id;
} 

inline int FM::getCutSize(){
    int cut_size = 0;
    for(size_t i=0;i<net_num;++i){
        if(cell_num_in_net[0].at(i) && cell_num_in_net[1].at(i))
            ++cut_size;
    }
    return cut_size;
}

void FM::run(){
    //clock_t start = clock();
    //clock_t temp;
    initialize();
    //std::cout<<"initialize takes "<<double(clock()-start)/CLOCKS_PER_SEC<<" s"<<std::endl;
    std::cout<<"initial cutsize:"<<getCutSize()<<std::endl;
    while(!stop){
        for(size_t i=0;i<cell_num;++i){
            //start = clock();
            size_t cell_id = chooseCell();
            //std::cout<<"chooseCell takes "<<double(clock()-start)/CLOCKS_PER_SEC<<" s"<<std::endl;
            //start = clock();
            updateGain(cell_id);

            //std::cout<<"updateGain takes "<<double(clock()-start)/CLOCKS_PER_SEC<<" s"<<std::endl;
        }
        std::cout<<std::endl;
        stop=true;
        std::cout<<"final cutsize:"<<min_cut<<std::endl;

#ifdef DEBUG
        assert(bucket[0].empty() && bucket[1].empty());            
#endif 
        initialize();
    }
}