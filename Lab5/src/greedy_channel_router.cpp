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
    frontier.resize(channel_width,nullptr);//plus edge
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
    
    frontier.push_front(edge[0].front());
    std::cout<<(*frontier.begin())->net_id<<std::endl;
    frontier.push_back(edge[1].front());
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
        stepA(i);
        writeGDT("test.gdt");
        std::cout<<"test"<<std::endl;
        break;

    }
}

void GreedyCR::stepA(int cur_col){
    int top_p = pins[0][cur_col];
    int bot_p = pins[1][cur_col];
    if(top_p==0 && bot_p==0){
        return;
    }
    if(top_p==bot_p){
        nets.at(top_p).jogs.push_back(new Jog(top_p,cur_col,frontier.front(),frontier.back()));
        return;
    }
    int top_dis = 1;
    int bot_dis = 1;
    Seg *top_ptr = nullptr;
    Seg *bot_ptr = nullptr;
    if(top_p!=0){    
        for(auto iter = std::next(frontier.begin());iter!=std::prev(frontier.end());++iter){
            std::cout<<"test"<<std::endl;
            if(*iter==nullptr){
                Seg *s = new Seg(top_p,cur_col);
                *iter = s;
                auto t_iter = tracks.begin();
                while(top_dis--!=0){
                    t_iter++;
                }
                t_iter->push_back(s);
                nets.at(top_p).jogs.push_back(new Jog(top_p,cur_col,frontier.front(),s));
                break;
            }else if((*iter)->net_id==top_p){
                top_ptr = *iter;
                break;
            }
            ++top_dis;
        }
    }
    std::cout<<"test2"<<std::endl;
    if(bot_p!=0){
        for(auto iter = std::next(frontier.rbegin());iter!=std::prev(frontier.rend());++iter){//reverse order
            if(*iter==nullptr){
                Seg *s = new Seg(bot_p,cur_col);
                *iter = s;
                auto t_iter = tracks.rbegin();
                while(bot_dis--!=0){
                    t_iter++;
                }
                t_iter->push_back(s);
                nets.at(bot_p).jogs.push_back(new Jog(bot_p,cur_col,s,frontier.back()));
                break;
            }else if((*iter)->net_id==bot_p){
                bot_ptr = *iter;
                break;
            }
            ++bot_dis;
        }
    }
    if(top_ptr!=nullptr && bot_ptr!=nullptr){
        if(top_dis+bot_dis>channel_width+1){//wire intersecting
            if(top_dis<bot_dis){
                nets.at(top_p).jogs.push_back(new Jog(top_p,cur_col,frontier.front(),top_ptr));
            }else{
                nets.at(bot_p).jogs.push_back(new Jog(bot_p,cur_col,bot_ptr,frontier.back()));
            }
        }else{
            nets.at(top_p).jogs.push_back(new Jog(top_p,cur_col,frontier.front(),top_ptr));
            nets.at(bot_p).jogs.push_back(new Jog(bot_p,cur_col,bot_ptr,frontier.back()));
        }
    }else if(top_ptr!=nullptr){
        nets.at(top_p).jogs.push_back(new Jog(top_p,cur_col,frontier.front(),top_ptr));
    }else if(bot_ptr!=nullptr){
        nets.at(bot_p).jogs.push_back(new Jog(bot_p,cur_col,bot_ptr,frontier.back()));
    }else{

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
    for(auto iter = tracks.begin();iter!=tracks.end();++iter){
        for(int j=0;j<(int)iter->size();++j){
            iter->at(j)->track_id = i;
        }
        ++i;
    }
    for(Net &n:nets){
        for(Jog *j:n.jogs){
            std::cout<<j->beg<<":"<<j->end<<std::endl;
            file << "b{" << j->net_id << " xy(" << 
                j->col - 0.1 << " " << j->beg->track_id << " " << 
                j->col - 0.1 << " " << j->end->track_id << " " <<
                j->col + 0.1 << " " << j->end->track_id << " " <<
                j->col + 0.1 << " " << j->beg->track_id << ")}" << std::endl;
        }
    }
    file << "}\n}\n";
    file.close();
}
