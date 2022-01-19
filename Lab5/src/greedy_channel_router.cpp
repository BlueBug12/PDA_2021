#include "greedy_channel_router.hpp"

inline bool no_pins(Net & n){ 
    if(n.cur_pos[0]<(int)n.pin_pos[0].size()-1||n.cur_pos[1]<(int)n.pin_pos[1].size()-1){
        return false;
    }
    return true;
}

GreedyCR::GreedyCR(const std::string filename):m_initial_channel_width(24),m_minimum_jog_length(1),m_steady_net_constant(1000){
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
    dummy->close = true;
    frontier.resize(channel_width,{dummy,0});//plus edge
    tracks.resize(channel_width);//plus edge
    column_number = pins[0].size();
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
        assert((int)nets.at(i).pin_pos[0].size()+(int)nets.at(i).pin_pos[1].size()>=2);
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
        for(int j=0;j<column_number;++j){
            Seg *s = new Seg(0);
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
    }
  fin_test.close();
#endif
}

void GreedyCR::run(){
    for(int i=0;i<column_number;++i){
        //std::cout<<i<<std::endl;
        nets.at(pins[0][i]).cur_pos[0]++;
        nets.at(pins[1][i]).cur_pos[1]++;
        stepA(i);
        stepB(i);
#ifdef DEBUG
        std::vector<int>count;
        count.resize(net_number+1,0);
        for(Iter iter = std::next(frontier.begin());iter!=std::prev(frontier.end());++iter){
           if(!iter->first->close)
               count[iter->first->net_id]++;
        }
        for(int i=1;i<(int)count.size();++i){
            //std::cout<<"net "<<i<<"("<<reverse[i]<<")"<<":"<<nets[i].counter<<","<<count[i]<<std::endl;
            assert(nets[i].counter==count[i]);
        }
#endif
        stepC(i);
        stepD(i);
        stepE(i);
        stepF(i);
#ifdef DEBUG
        count.clear();
        count.resize(net_number+1,0);
        for(Iter iter = std::next(frontier.begin());iter!=std::prev(frontier.end());++iter){
           if(iter->first!=dummy)
               count[iter->first->net_id]++;
        }
        for(int i=1;i<(int)count.size();++i){
            //std::cout<<"net\' "<<i<<"("<<reverse[i]<<")"<<":"<<nets[i].counter<<","<<count[i]<<std::endl;
            //assert(nets[i].counter==count[i]);
        }
#endif
        //writeGDT("test.gdt");

    }
    bool flag = false;
    int i = column_number;
    for(Iter iter = std::next(frontier.begin());iter!=std::prev(frontier.end());++iter){
        if(!iter->first->close){
            flag = true;
            break;
        }
    }
    while(flag){
        flag = false;
        stepA(i);
        stepB(i);
        stepC(i);
        stepD(i);
        stepE(i);
        stepF(i);    
        for(Iter iter = std::next(frontier.begin());iter!=std::prev(frontier.end());++iter){
            if(!iter->first->close){
                flag = true;
                break;
            }
        }
        ++i;
    }
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
        if(no_pins(nets.at(top_p))){
            for(auto iter = frontier.begin();iter!=frontier.end();++iter){
                iter->second = top_p;
                if(iter->first->net_id==top_p){
                    iter->first->close = true;
                }
            }
            nets.at(top_p).counter = 0;
        }else{
            Net & n = nets.at(top_p);
            for(auto iter = std::next(frontier.begin());iter!=std::prev(frontier.end());++iter){
                iter->second = top_p;
                if(iter->first->net_id==top_p&&!iter->first->close){
                    iter->first->close = true;
                    nets.at(top_p).counter--;
                }
            }
            int top_dis = 10000000;
            int bot_dis = 10000000;
            if(n.cur_pos[0]<(int)n.pin_pos[0].size()-1&&!n.pin_pos[0].empty()){
                top_dis = n.pin_pos[0][n.cur_pos[0]+1];
            }
            if(n.cur_pos[1]<(int)n.pin_pos[1].size()-1&&!n.pin_pos[1].empty()){
                bot_dis = n.pin_pos[1][n.cur_pos[1]+1];
            }
            /*
            if(n.cur_pos[0]<(int)n.pin_pos[0].size()-1){
                top_dis = n.pin_pos[0][n.cur_pos[0]];
            }
            if(n.cur_pos[1]<(int)n.pin_pos[1].size()-1){
                bot_dis = n.pin_pos[1][n.cur_pos[1]];
            }*/
            if(top_dis<bot_dis){//move up
                for(Iter iter=std::next(frontier.begin());iter!=std::prev(frontier.end());++iter){
                    if(iter->first==dummy){
                        Seg *s = new Seg(top_p,cur_col);
                        n.counter++;
                        n.segments.push_back(s);
                        iter->first = s;
                        auto t_iter = tracks.begin();
                        std::advance(t_iter,std::distance(frontier.begin(),iter));
                        t_iter->push_back(s);
                        break;
                    }
                }
            }else{//move down 
                for(auto iter=std::next(frontier.rbegin());iter!=std::prev(frontier.rend());++iter){
                    if(iter->first==dummy){
                        Seg *s = new Seg(top_p,cur_col);
                        n.counter++;
                        n.segments.push_back(s);
                        iter->first = s;
                        auto t_iter = tracks.rbegin();
                        std::advance(t_iter,std::distance(frontier.rbegin(),iter));
                        t_iter->push_back(s);
                        break;
                    }
                }
            }
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
                addJog(top_p,frontier.begin(),std::next(beg_it),cur_col,frontier.front().first,top_ptr);
                end_it = std::prev(frontier.end());
            }else{
                addJog(bot_p,std::prev(end_it),frontier.end(),cur_col,bot_ptr,frontier.back().first);
                beg_it = std::next(frontier.begin());
            }
        }else{
            addJog(top_p,frontier.begin(),std::next(beg_it),cur_col,frontier.front().first,top_ptr);
            addJog(bot_p,std::prev(end_it),frontier.end(),cur_col,bot_ptr,frontier.back().first);

        }
    }else if(top_ptr!=nullptr){
        if(top_dis+bot_dis<channel_width+1){
            addJog(top_p,frontier.begin(),std::next(beg_it),cur_col,frontier.front().first,top_ptr);
        }else{
            beg_it = std::next(frontier.begin());
        }
    }else if(bot_ptr!=nullptr){
        if(top_dis+bot_dis<channel_width+1){
            addJog(bot_p,std::prev(end_it),frontier.end(),cur_col,bot_ptr,frontier.back().first);
        }else{
            end_it = std::prev(frontier.end());
        }
    }else{

    }
#ifdef DEBUG
    assert(std::distance(frontier.begin(),beg_it)<=std::distance(frontier.begin(),end_it));
#endif
}

void GreedyCR::addJog(int net_id, Iter beg, Iter end, int cur_col, Seg *top, Seg *bot){
    Net & n = nets.at(net_id);
    n.jogs.push_back(new Jog(net_id,cur_col,top,bot));
    std::for_each(beg,end,[&](std::pair<Seg *,int> & p){p.second=net_id;});
    if(n.counter==1 && no_pins(n)){
        n.counter = 0;
        top->close = true;
        bot->close = true;
    }
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
        if(iter_i->first==dummy){
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
            if(iter_j->first==dummy){
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
            Iter b_it = frontier.begin();
            Iter e_it = frontier.begin();

            for(;b_it->first!=beg;++b_it){
                beg_dis++;
            }
            for(;e_it->first!=end;++e_it){
                end_dis++;
            }
            end_dis = channel_width + 1 - end_dis;
            
#ifdef DEBUG
            assert(beg!=nullptr);
            assert(end!=nullptr);
            assert(beg->net_id==end->net_id);
#endif
            int n_id = beg->net_id;
            nets.at(n_id).jogs.push_back(new Jog(n_id,cur_col,beg,end));
            std::for_each(b_it,std::next(e_it),[&](std::pair<Seg*,int> & p){p.second=n_id;});
            if(no_pins(nets.at(n_id))){
                if(!beg->close){
                    beg->close = true;
                    nets.at(n_id).counter -= 1;
                }
                if(!end->close){
                    end->close = true;
                    nets.at(n_id).counter -= 1;
                }
                if(nets.at(n_id).counter>=1){
                    end->close = false;
                    nets.at(n_id).counter += 1;
                }
            }else if(beg_dis==0){
                if(!beg->close){
                    beg->close = true;
                    nets.at(n_id).counter -= 1;
                }
            }else if(end_dis==0){
                if(!end->close){
                    end->close = true;
                    nets.at(n_id).counter -= 1;
                }
            }else if(beg->close){
                if(!end->close){
                    end->close = true;
                    nets.at(n_id).counter -= 1;
                }
            }else if(end->close){
                if(!beg->close){
                    beg->close = true;
                    nets.at(n_id).counter -= 1;
                }
            }else if(beg_dis<=end_dis){
                if(!end->close){
                    end->close = true;
                    nets.at(n_id).counter -= 1;
                }
            }else{
                if(!beg->close){
                    beg->close = true;
                    nets.at(n_id).counter -= 1;
                }
            }
        }
    }
}

void GreedyCR::stepC(int cur_col){  
    std::map<int,int>m;
    for(Iter it_i = std::next(frontier.begin());it_i!=std::prev(frontier.end());++it_i){
        Seg *s = it_i->first;
        int n_id = s->net_id;
        Net & n = nets[s->net_id];
        if(s->close)
            continue;
        if(n.counter>=2){
            Iter target_it = frontier.begin();
            int num = ++m[s->net_id];
            if(num<=n.counter/2){//move down
                //std::cout<<"down "<<s->net_id<<std::endl;
                Iter back = target_it;
                for(Iter it_j = std::next(it_i);it_j!=std::prev(frontier.end());++it_j){
                    if(it_j->second==0||it_j->second==n_id){
                        target_it = it_j;
                        if(it_j->first==dummy||it_j->first->net_id==n_id){
                            back = it_j;
                        }
                    }else{
                        break;
                    }
                }
                if(target_it->first!=dummy&&target_it->first->net_id!=n_id){
                    target_it = back;
                }
                if(target_it!=frontier.begin() && std::distance(it_i,target_it)>=m_minimum_jog_length){
                    //std::cout<<"down "<<s->net_id<<" success"<<std::endl;
                    target_it->first = new Seg(s->net_id,cur_col);

                    auto t_iter = tracks.begin();
                    std::advance(t_iter,std::distance(frontier.begin(),target_it));
                    t_iter->push_back(target_it->first);

                    n.segments.push_back(target_it->first);
                    Jog *j = new Jog(s->net_id,cur_col,it_i->first,target_it->first);
                    it_i->first->close = true;
                    n.jogs.push_back(j);
                    std::for_each(it_i,std::next(target_it),[&](std::pair<Seg*,int> & p){p.second=s->net_id;});
                    while(it_i!=target_it){
                        ++it_i;
                    }
                }
            }else{//move up
                //std::cout<<"up "<<s->net_id<<std::endl;
                Iter back = frontier.begin();
                for(Iter it_j = std::prev(it_i);it_j!=frontier.begin();){
                    if(it_j->second==0||it_j->second==n_id){
                        target_it = it_j;
                        if(it_j->first==dummy||it_j->first->net_id==n_id){
                            back = it_j;
                        }
                    }else{
                        break;
                    }
                    it_j = std::prev(it_j);
                }
                if(target_it->first!=dummy&&target_it->first->net_id!=n_id){
                    target_it = back;
                }
                if(target_it!=frontier.begin() && std::distance(target_it,it_i)>=m_minimum_jog_length){
                    //std::cout<<"up "<<s->net_id<<" success"<<std::endl;
                    target_it->first = new Seg(s->net_id,cur_col);

                    auto t_iter = tracks.begin();
                    std::advance(t_iter,std::distance(frontier.begin(),target_it));
                    t_iter->push_back(target_it->first);

                    n.segments.push_back(target_it->first);
                    Jog *j = new Jog(s->net_id,cur_col,target_it->first,it_i->first);
                    it_i->first->close = true;
                    n.jogs.push_back(j);
                    std::for_each(target_it,std::next(it_i),[&](std::pair<Seg*,int> & p){p.second=s->net_id;});
                }
            }
        }
    }
}

void GreedyCR::stepD(int cur_col){
    for(Iter it_i = std::next(frontier.begin());it_i!=std::prev(frontier.end());++it_i){
        int n_id = it_i->first->net_id;
        Net & n = nets.at(n_id);
        if(it_i->first->close)
            continue;
        Iter target_it = frontier.begin();
        if(n.counter==1){
            int top_dis = 10000000;
            int bot_dis = 10000000;
            if(n.cur_pos[0]<(int)n.pin_pos[0].size()-1&&!n.pin_pos[0].empty()){
                top_dis = n.pin_pos[0][n.cur_pos[0]+1];
            }
            if(n.cur_pos[1]<(int)n.pin_pos[1].size()-1&&!n.pin_pos[1].empty()){
                bot_dis = n.pin_pos[1][n.cur_pos[1]+1];
            }
            //std::cout<<"stepD "<<n_id<<":"<<top_dis<<","<<bot_dis<<std::endl;
            if(top_dis<bot_dis  && top_dis <= m_steady_net_constant){//move up
                Iter back = frontier.begin();
                for(Iter it_j = std::prev(it_i);it_j!=frontier.begin();){
                    if(it_j->second==0){
                        target_it = it_j;
                        if(it_j->first==dummy){
                            back = it_j;
                        }
                    }else{
                        break;
                    }
                    it_j = std::prev(it_j);
                }
                if(target_it->first!=dummy){
                    target_it = back;
                }
                if(target_it!=frontier.begin() && std::distance(target_it,it_i)>=m_minimum_jog_length){
                    //std::cout<<"D up "<<n_id<<" success"<<std::endl;
                    target_it->first = new Seg(n_id,cur_col);

                    auto t_iter = tracks.begin();
                    std::advance(t_iter,std::distance(frontier.begin(),target_it));
                    t_iter->push_back(target_it->first);

                    n.segments.push_back(target_it->first);
                    Jog *j = new Jog(n_id,cur_col,target_it->first,it_i->first);
                    it_i->first->close = true;
                    n.jogs.push_back(j);
                    std::for_each(target_it,std::next(it_i),[&](std::pair<Seg*,int> & p){p.second=n_id;});
                }
                
            }else if(top_dis>bot_dis  && bot_dis <= m_steady_net_constant){//move down
                Iter back = target_it;
                for(Iter it_j = std::next(it_i);it_j!=std::prev(frontier.end());++it_j){
                    if(it_j->second==0){
                        target_it = it_j;
                        if(it_j->first==dummy){
                            back = it_j;
                        }
                    }else{
                        break;
                    }
                }
                if(target_it->first!=dummy){
                    target_it = back;
                }
                if(target_it!=frontier.begin() && std::distance(it_i,target_it)>=m_minimum_jog_length){
                    //std::cout<<"D down "<<n_id<<" success"<<std::endl;
                    target_it->first = new Seg(n_id,cur_col);

                    auto t_iter = tracks.begin();
                    std::advance(t_iter,std::distance(frontier.begin(),target_it));
                    t_iter->push_back(target_it->first);

                    n.segments.push_back(target_it->first);
                    Jog *j = new Jog(n_id,cur_col,it_i->first,target_it->first);
                    it_i->first->close = true;
                    n.jogs.push_back(j);
                    std::for_each(it_i,std::next(target_it),[&](std::pair<Seg*,int> & p){p.second=n_id;});
                }
            
            }

        }
    }

}

void GreedyCR::stepE(int cur_col){
}

void GreedyCR::stepF(int cur_col){
   for(auto iter = std::next(frontier.begin());iter!=std::prev(frontier.end());++iter){
       Seg *s = iter->first;
       if(s->close){
           *iter= {dummy,0};
       }else{
           s->end++;
           iter->second = 0;
       }
   } 
}

void GreedyCR::writeOutput(const std::string & filename){
    
    std::ofstream file{filename};
    if(!file){
        std::cerr << "Error: can not open file "<<filename<<std::endl;
        exit(1);
    }
    int i = 0;
    for(auto iter = tracks.rbegin();iter!=tracks.rend();++iter){
        for(int j=0;j<(int)iter->size();++j){
            iter->at(j)->track_id = i;
        }
        ++i;
    }
    for(int j = 1;j<(int)nets.size();++j){
        Net & n = nets[j];
        file << ".begin "<< reverse[j] << std::endl;
        for(Jog *j:n.jogs){
            file << ".V " << j->col << " "<< j->end->track_id << " " << j->beg->track_id <<std::endl;  
        }
        for(Seg *s:n.segments){
            if(s->beg==s->end)
                continue;
            file << ".H " << s->beg << " " << s-> track_id << " " << s->end << std::endl;
        }
        file << ".end"<<std::endl;
    }

    file.close();
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
