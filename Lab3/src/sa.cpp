#include "sa.hpp"

void SA::buildSP(const std::string block_name, const std::string net_name, double alpha){
    if(sp!=nullptr)
        delete sp;
    sp = new SP(block_name,net_name,alpha);
}


double SA::acceptance(double old_e, double new_e, double temperature){
    return std::exp((old_e - new_e)/(temperature)); 
}


void SA::run(){

    double cur_t = m_initial_t;
    int cur_w, cur_h, cur_hpwl, cur_area;
    int ori_temp;
    double cur_e = sp->getCost(cur_w, cur_h, cur_hpwl, cur_area,ori_temp);
    b_cost = cur_e;
    b_origin_cost = ori_temp;
    b_width = cur_w;
    b_height = cur_h;
    b_hpwl = cur_hpwl;
    b_area = cur_area;
    pos_x = sp->pos[0];
    pos_y = sp->pos[1];
    dim_w = sp->dim[0];
    dim_h = sp->dim[1];

#ifdef VERBOSE
    std::cout<<"initial hpwl:"<<b_hpwl<<std::endl;
    std::cout<<"initial area:"<<b_area<<std::endl;
    std::cout<<"initial cost:"<<b_origin_cost<<std::endl;
#endif

    int accept_good = 0;
    int accept_bad = 0;
    int reject_bad = 0;
    int local_ag = 0;
    int local_ab = 0;
    int local_rb = 0;
    int iter = 0;
    double reject_rate = 0.0;
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<double> distr(0, 1);
    std::uniform_int_distribution<> dis(1,3);
    while(reject_rate <= 0.95 && cur_t > m_final_t){
        for(int i=0;i<m_markov_iter;++i){
            int option = dis(eng);
            if(option==1){
                sp->op1();
            }else if(option==2){
                sp->op2();
            }else{
                sp->op3();
            }
			double new_e = sp->getCost(cur_w, cur_h, cur_hpwl, cur_area,ori_temp);
            if(new_e < cur_e){
                cur_e = new_e;
                local_ag += 1;
            }else{
                double prob = acceptance(cur_e,new_e,cur_t);
                if(prob > distr(eng)){
                    cur_e = new_e;
                    local_ab += 1;
                }else{
                    sp->reverse();
                    local_rb += 1;
                }
            }

            if(b_cost > cur_e){
                b_cost = cur_e;
                b_origin_cost = ori_temp;
                b_width = cur_w;
                b_height = cur_h;
                b_hpwl = cur_hpwl;
                b_area = cur_area;
                pos_x = sp->pos[0];
                pos_y = sp->pos[1];
                dim_w = sp->dim[0];
                dim_h = sp->dim[1];
            }
        }

        int den = local_ag + local_ab + local_rb;
        reject_rate = (double)local_rb/den;
        accept_good += local_ag;
        accept_bad += local_ab;
        reject_bad += local_rb;
        local_ag = 0;
        local_ab = 0;
        local_rb = 0;
        ++iter;
        cur_t *= m_descent_rate;
    }

    if(b_width<=sp->outline_w && b_height<=sp->outline_h){
        pass = true;
    }else{
        pass = false;
    }
    
#ifdef VERBOSE
    std::cout<<"Outline width:"<<sp->outline_w<<" floorplan width:"<<b_width<<std::endl;
    std::cout<<"Outline height:"<<sp->outline_h<<" floorplan height:"<<b_height<<std::endl;
    std::cout<<"accept good:"<<accept_good<<std::endl;
    std::cout<<"accept bad:"<<accept_bad<<std::endl;
    std::cout<<"reject bad:"<<reject_bad<<std::endl;
    std::cout<<"lowest energy:"<<b_origin_cost<<std::endl;
    std::cout<<"final area:"<<b_area<<std::endl;
    std::cout<<"final hpwl:"<<b_hpwl<<std::endl;
#endif 
}

void SA::updateResult(){
#ifdef DISPLAY
    std::cout<<"cost = "<<b_origin_cost<<std::endl;
#endif

    if(pass && b_origin_cost < global_cost){
#ifdef DISPLAY
        std::cout<<"update best result!"<<std::endl;
#endif
        global_cost = b_origin_cost;
        global_pos_x = pos_x;
        global_pos_y = pos_y;
        global_dim_w = dim_w;
        global_dim_h = dim_h;
        global_hpwl = b_hpwl;
        global_area = b_area;
        global_width = b_width;
        global_height = b_height;
#ifdef DEBUG
        assert(sp->block_num+sp->terminal_num==(int)global_pos_x.size());
        assert(sp->block_num==(int)global_dim_w.size());
#endif
    }
    pass = false;
}

void SA::writeResult(const std::string& file_name, float time){
    std::ofstream fout{file_name};
    if(!fout){
        std::cerr << "Error: can not open file"<<file_name<<std::endl;
        exit(1);
    }

    fout << global_cost <<std::endl;
    fout << global_hpwl << std::endl;
    fout << global_area << std::endl;
    fout << global_width << " "<< global_height << std::endl;
    fout << time << std::endl;
    std::vector<std::string>name;
    sp->nameList(name);
    
#ifdef DEBUG
    assert(sp->block_num+sp->terminal_num==(int)global_pos_x.size());
#endif
    for(int i=0;i<(int)name.size();++i){
        fout << name[i] << " " <<  global_pos_x[i] << " " << global_pos_y[i] << " " << global_pos_x[i]+global_dim_w[i] << " " << global_pos_y[i]+global_dim_h[i] <<std::endl;
    }

    fout.close();
}
