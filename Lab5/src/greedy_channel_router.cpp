#include "greedy_channel_router.hpp"

GreedyCR::GreedyCR(const std::string filename):m_initial_channel_width(20),m_minimum_jog_length(5),m_steady_net_constant(10){
    parser(filename);
    initialize();
}

GreedyCR::GreedyCR(const std::string filename, int icw, int mjl, int snc):m_initial_channel_width(icw),m_minimum_jog_length(mjl),m_steady_net_constant(snc){
    parser(filename);
    initialize();
}
void GreedyCR::initialize(){
    channel_width = m_initial_channel_width;
    dummy = new Seg(0);
    frontier.resize(channel_width,{dummy,0});//plus edge
    tracks.resize(channel_width);//plus edge
    column_number = pins[0].size();
    columns.resize(column_number);
    net_number = dict.size()-1;//except 0
    F(net_number+1){
        Net n;
        nets.push_back(n);
    }
    for(int i=0;i<2;++i){
        for(int j=0;j<(int)pins[i].size();++j){
            if(pins[i][j]!=0){
                nets.at(pins[i][j]).pin_pos[i].push_back(j);
            }
        }
    }
    
#ifdef DEBUG
    for(int i=1;i<(int)nets.size();++i){
        for(int j=0;j<2;++j){
            for(int k=0;k<(int)nets.at(i).pin_pos[j].size();++k){
                assert(pins[j][nets[i].pin_pos[j][k]]==i);
            }
        }
    }
#endif
    
    std::vector<Seg *>edge[2];
    for(int i=0;i<2;++i){
        for(int j=0;j<column_number;++j){
            Seg *s = new Seg(pins[i][j]);
            edge[i].push_back(s);
        }
    }
    
    frontier.push_front({edge[0].front(),pins[0][0]});
    frontier.push_back({edge[1].front(),pins[1][0]});
    tracks.push_front(std::move(edge[0]));
    tracks.push_back(std::move(edge[1]));
}
void GreedyCR::parser(const std::string & filename){
	std::ifstream fin{filename};
    if(!fin){
        std::cerr << "Error: can not open file "<<filename<<std::endl;
        exit(1);
    }
    std::string str;
    dict[0] = 0;
    F(2){
        std::getline(fin,str);
        std::stringstream ss(str);
        while(ss >> str){
            int pin = std::stoi(str);
            auto iter = dict.find(pin);
            if(iter==dict.end()){
                dict[pin] = dict.size();
                pin = dict.size()-1;
            }else{
                pin = iter->second;
            }
            pins[i].push_back(pin);    
        }
    } 
	fin.close();
    reverse.resize(dict.size());
    for(auto & pair: dict){
        reverse.at(pair.second) = pair.first;
    }
#ifdef DEBUG
    assert(pins[0].size()==pins[1].size());

	std::ifstream fin_test{filename};
    F(2){
        std::getline(fin_test,str);
        std::stringstream ss(str);
        for(int j =0;j<(int)pins[i].size();++j){
            ss >> str;
            int pin = std::stoi(str);
            assert(pin == reverse[pins[i][j]]);
        }
        /*
        for(int j=0;j<pins[0].size();++j){
            std::cout << pins[i][j] << " ";
        }
        std::cout << std::endl;*/
    }
  fin_test.close();
#endif
}

void GreedyCR::run(){
    for(int i=0;i<column_number;++i){
        nets.at(pins[0][i]).cur_pos[0]++;
        nets.at(pins[1][i]).cur_pos[1]++;
        stepA(i);
        stepB(i);
        stepC(i);
        stepD(i);
        stepE(i);
        stepF(i);
        writeGDT("test.gdt");
        //break;

    }
    /*
    for(int i=0;i<nets.size();++i){

        std::cout<<"n"<<i<<":"<<nets[i].counter<<std::endl;
    }*/
}

void GreedyCR::stepA(int cur_col){
    int top_p = pins[0][cur_col];
    int bot_p = pins[1][cur_col];
    beg_it = std::next(frontier.begin());
    end_it = std::prev(frontier.end());
    if(top_p==0 && bot_p==0){
        return;
    }
    if(top_p==bot_p){
        nets.at(top_p).jogs.push_back(new Jog(top_p,cur_col,frontier.front().first,frontier.back().first));
        beg_it = std::prev(end_it);
        for(auto iter = frontier.begin();iter!=frontier.end();++iter){
            iter->second = top_p;
        }
        return; 
    }
    int top_dis = 1;
    int bot_dis = 1;
    Seg *top_ptr = nullptr;
    Seg *bot_ptr = nullptr;
    if(top_p!=0){    
        for(auto iter = std::next(frontier.begin());iter!=std::prev(frontier.end());++iter){
#ifdef DEBUG
            assert(iter->first!=nullptr);
#endif
            if(iter->first->net_id==0){
                Seg *s = new Seg(top_p,cur_col);
                nets.at(top_p).counter++;
                nets.at(top_p).segments.push_back(s);
                iter->first = s;
                auto t_iter = tracks.begin();
                std::advance(t_iter,top_dis);
#ifdef DEBUG
                assert(std::distance(tracks.begin(),t_iter)==std::distance(frontier.begin(),iter));
#endif
                t_iter->push_back(s);
                nets.at(top_p).jogs.push_back(new Jog(top_p,cur_col,frontier.front().first,s));
                std::for_each(frontier.begin(),std::next(iter),[&](std::pair<Seg*,int> & p){p.second=top_p;});
                beg_it = iter;
                break;
            }else if(iter->first->net_id==top_p){
                beg_it = iter;
                top_ptr = iter->first;
                break;
            }
            ++top_dis;
        }
    }
    if(bot_p!=0){
        for(auto iter = std::next(frontier.rbegin());iter!=std::prev(frontier.rend());++iter){//reverse order
#ifdef DEBUG
            assert(iter->first!=nullptr);
#endif
            if(iter->first->net_id==0){//empty track
                Seg *s = new Seg(bot_p,cur_col);
                nets.at(bot_p).counter++;
                nets.at(bot_p).segments.push_back(s);
                iter->first = s;
                auto t_iter = tracks.rbegin();
                std::advance(t_iter,bot_dis);
#ifdef DEBUG
                assert(std::distance(tracks.rbegin(),t_iter)==std::distance(frontier.rbegin(),iter));
#endif
                t_iter->push_back(s);
                nets.at(bot_p).jogs.push_back(new Jog(bot_p,cur_col,s,frontier.back().first));
                std::for_each(frontier.rbegin(),std::next(iter),[&](std::pair<Seg*,int> & p){p.second=bot_p;});
                end_it = frontier.begin();
                std::advance(end_it,channel_width+2-bot_dis);
#ifdef DEBUG
                assert(*std::prev(end_it)==*iter);
#endif
                break;
            }else if(iter->first->net_id==bot_p){
                end_it = frontier.begin();
                std::advance(end_it,channel_width+2-bot_dis);
                bot_ptr = iter->first;
                break;
            }
            ++bot_dis;
        }
    }
    if(top_ptr!=nullptr && bot_ptr!=nullptr){
        if(top_dis+bot_dis>channel_width+1){//wire intersecting
            if(top_dis<bot_dis){
                nets.at(top_p).jogs.push_back(new Jog(top_p,cur_col,frontier.front().first,top_ptr));
                std::for_each(frontier.begin(),std::next(beg_it),[&](std::pair<Seg*,int> & p){p.second=top_p;});
                end_it = std::prev(frontier.end());
            }else{
                nets.at(bot_p).jogs.push_back(new Jog(bot_p,cur_col,bot_ptr,frontier.back().first));
                std::for_each(end_it,frontier.end(),[&](std::pair<Seg*,int> & p){p.second=bot_p;});
                beg_it = std::next(frontier.begin());
            }
        }else{
            nets.at(top_p).jogs.push_back(new Jog(top_p,cur_col,frontier.front().first,top_ptr));
            std::for_each(frontier.begin(),std::next(beg_it),[&](std::pair<Seg*,int> & p){p.second=top_p;});
            nets.at(bot_p).jogs.push_back(new Jog(bot_p,cur_col,bot_ptr,frontier.back().first));
            std::for_each(end_it,frontier.end(),[&](std::pair<Seg*,int> & p){p.second=bot_p;});
        }
    }else if(top_ptr!=nullptr){
        if(top_dis+bot_dis<channel_width+1){
            nets.at(top_p).jogs.push_back(new Jog(top_p,cur_col,frontier.front().first,top_ptr));
            std::for_each(frontier.begin(),std::next(beg_it),[&](std::pair<Seg*,int> & p){p.second=top_p;});
        }else{
            beg_it = std::next(frontier.begin());
        }
    }else if(bot_ptr!=nullptr){
        if(top_dis+bot_dis<channel_width+1){
            nets.at(bot_p).jogs.push_back(new Jog(bot_p,cur_col,bot_ptr,frontier.back().first));
            std::for_each(end_it,frontier.end(),[&](std::pair<Seg*,int> & p){p.second=bot_p;});
        }else{
            end_it = std::prev(frontier.end());
        }
    }else{
        //beg_it = std::next(frontier.begin());
        //end_it = std::prev(frontier.end());
    }
#ifdef DEBUG
    assert(std::distance(frontier.begin(),beg_it)<=std::distance(frontier.begin(),end_it));
#endif
}

inline bool no_pins(Net & n){ 
    if(n.cur_pos[0]<(int)n.pin_pos[0].size()-1||n.cur_pos[1]<(int)n.pin_pos[1].size()-1){
        return false;
    }
    return true;
}

void GreedyCR::stepB(int cur_col){
    if(beg_it==end_it){
        return;
    }
    //pair <empty_track,jog_length>
    std::map<std::pair<int,int>,std::vector<std::pair<Seg *,Seg *>>,std::greater<std::pair<int,int>>>m;
#ifdef DEBUG
    for(auto iter = frontier.begin();iter!=frontier.end();++iter){
        assert(iter->first!=nullptr);
    }
#endif
    for(auto iter_i = beg_it;iter_i!=std::prev(end_it);++iter_i){
        Seg *s_i = iter_i->first;
        if(nets.at(s_i->net_id).counter==0){
            continue;
        }
        int target_id = s_i->net_id;
        int jog_len = 0;
        int free_tracks = 0;
        std::vector<std::pair<Seg *,Seg *>>pattern;
        for(auto iter_j = iter_i;iter_j!=std::prev(end_it);++iter_j){
            Seg *s_j = iter_j->first;
#ifdef DEBUG
            assert(s_j!=nullptr);
#endif
            if(nets.at(s_j->net_id).counter==0){
                continue;
            }
            target_id = s_j->net_id;
            for(auto iter_k = std::next(iter_j);iter_k!=end_it;++iter_k){
                Seg *s_k = iter_k->first;
#ifdef DEBUG
                assert(s_k!=nullptr);
                assert(std::distance(frontier.begin(),iter_k)<std::distance(frontier.begin(),end_it));
#endif
                if(target_id==s_k->net_id){
                   jog_len += std::distance(iter_j,iter_k);
                   free_tracks++;
                   Net & n = nets.at(target_id);
                   if(no_pins(n)){
                        free_tracks++; 
                   }
#ifdef DEBUG
                   assert(s_j!=nullptr);
                   assert(s_k!=nullptr);
#endif
                   pattern.push_back({s_j,s_k});
                   iter_j = std::prev(iter_k);
                   break;
                }
            }
            /*
            if(iter_j==end_it){
                break;    
            }*/
        }
        if(!pattern.empty()){
            m[{free_tracks,jog_len}] = std::move(pattern);
        }
    }
    if(!m.empty()){
        std::vector<std::pair<Seg *,Seg*>>&v = m.begin()->second;
        for(int i=0;i<(int)v.size();++i){
            Seg *beg = v[i].first;
            Seg *end = v[i].second;
            int beg_dis = 0;
            int end_dis = 0;
            for(auto iter = frontier.begin();iter->first!=beg;++iter){
                beg_dis++;
            }
            for(auto iter = frontier.rbegin();iter->first!=end;++iter){
                end_dis++;
            }
            
#ifdef DEBUG
            assert(beg!=nullptr);
            assert(end!=nullptr);
#endif
            int n_id = beg->net_id;
            nets.at(n_id).jogs.push_back(new Jog(n_id,cur_col,beg,end));
            if(no_pins(nets.at(n_id))){
                beg->close = true;
                end->close = true;
                nets.at(n_id).counter -= 2;
            }else if(beg_dis==0){
                beg->close = true;
                nets.at(n_id).counter -= 1;
            }else if(end_dis==0){
                end->close = true;
                nets.at(n_id).counter -= 1;
            }else if(beg_dis<=end_dis){
                end->close = true;
                nets.at(n_id).counter -= 1;
            }else{
                beg->close = true;
                nets.at(n_id).counter -= 1;
            }
        }
    }
}

void GreedyCR::stepC(int cur_col){

}

void GreedyCR::stepD(int cur_col){
    
}

void GreedyCR::stepE(int cur_col){

}

void GreedyCR::stepF(int cur_col){
   for(auto iter = std::next(frontier.begin());iter!=std::prev(frontier.end());++iter){
       Seg *s = iter->first;
       if(s->net_id==0 || s->close){
           iter->first = dummy;
           continue;
       }
       Net & n = nets.at(s->net_id);
       if(!s->close || !no_pins(n)){
#ifdef DEBUG
           if(iter!=frontier.begin() && iter!=std::prev(frontier.end())){
               assert(s->beg!=-1);
               assert(s->end!=-1);
           }
#endif
           s->end++;
       }else{
           iter->first = dummy;
       }
   } 
}

void GreedyCR::writeGDT(const std::string & filename){
    std::ofstream file{filename};
    if(!file){
        std::cerr << "Error: can not open file "<<filename<<std::endl;
        exit(1);
    }
    file<<"gds2{600\nm=2018-09-14 14:26:15 a=2018-09-14 14:26:15\nlib 'asap7sc7p5t_24_SL' 0.1 1e-5\ncell{c=2018-09-14 14:26:15 m=2018-09-14 14:26:15 'AND2x2_ASAP7_75t_SL'\n";
    int i = 0;
    for(auto iter = tracks.rbegin();iter!=tracks.rend();++iter){
        for(int j=0;j<(int)iter->size();++j){
            iter->at(j)->track_id = i;
        }
        ++i;
    }
    for(Net &n:nets){
        for(Jog *j:n.jogs){
            file << "b{" << j->net_id << " xy(" << 
                j->col - 0.1 << " " << j->beg->track_id << " " << 
                j->col - 0.1 << " " << j->end->track_id << " " <<
                j->col + 0.1 << " " << j->end->track_id << " " <<
                j->col + 0.1 << " " << j->beg->track_id << ")}" << std::endl;
        }
        for(Seg *s:n.segments){
            file << "b{" << s->net_id << " xy(" <<
                s->beg - 0.1 << " " << s->track_id - 0.1 << " " <<
                s->beg - 0.1 << " " << s->track_id + 0.1 << " " <<
                s->end - 0.1 << " " << s->track_id + 0.1 << " " <<
                s->end - 0.1 << " " << s->track_id - 0.1 << ")}" << std::endl;
        }
    }
    file << "}\n}\n";
    file.close();
}
