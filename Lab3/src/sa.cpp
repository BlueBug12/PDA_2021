#include "sa.hpp"

void SA::buildSP(const std::string block_name, const std::string net_name, double alpha){
    sp = new SP(block_name,net_name,alpha);
}


double SA::acceptance(double old_e, double new_e, double temperature){
    //return std::exp((old_e - new_e)/(10000000)); 
    return std::exp((old_e - new_e)/(temperature)); 
}

double skew(int w, int h,int o_w,int o_h){
    double r = (double)(w*o_h) / (double)(h*o_w);
    if(r < 1)
        r = 1/r;
    if(w>o_w){
        r*=1.1;
    }
    if(h>o_h){
        r*=1.1;
    }
    if(w<=o_w && h <= o_h){
        r*=0.8;
    }
    return r;
}

void SA::run(){

    double cur_t = m_initial_t;
    int cur_w, cur_h, cur_hpwl, cur_area;
    double cur_e = sp->getCost(cur_w, cur_h, cur_hpwl, cur_area);
    b_origin_cost = cur_e;
    cur_e *= skew(cur_w,cur_h,sp->outline_w,sp->outline_h);
    b_cost = cur_e;
    b_width = cur_w;
    b_height = cur_h;
    b_hpwl = cur_hpwl;
    b_area = cur_area;
    std::cout<<"initial hpwl:"<<b_hpwl<<std::endl;
    std::cout<<"initial area:"<<b_area<<std::endl;
    std::cout<<"initial cost:"<<b_origin_cost<<std::endl;

    pos_x = sp->pos[0];
    pos_y = sp->pos[1];
    dim_w = sp->dim[0];
    dim_h = sp->dim[1];

    int accept_good = 0;
    int accept_bad = 0;
    int reject_bad = 0;
    int local_ag = 0;
    int local_ab = 0;
    int local_rb = 0;
    int iter = 0;
    int logger_iter = 100;
    int better =0;
    double reject_rate = 0.0;
    double origin_e;
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<double> distr(0, 1);
    std::uniform_int_distribution<> dis(1,3);
    while(reject_rate <= 0.95){
        for(int i=0;i<m_markov_iter;++i){
            int option = dis(eng);
            if(option==1){
                sp->op1();
            }else if(option==2){
                sp->op2();
            }else{
                sp->op3();
            }
            /*
            int r = std::rand()%100;
            if(r<35){
                sp->op3();
            }
            if(r<50){
                sp->op2();
            }else{
                sp->op1();
            }*/
			origin_e = sp->getCost(cur_w, cur_h, cur_hpwl, cur_area);
            double new_e = origin_e*skew(cur_w,cur_h,sp->outline_w,sp->outline_h);
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
                b_origin_cost = origin_e;
                b_width = cur_w;
                b_height = cur_h;
                b_hpwl = cur_hpwl;
                b_area = cur_area;
                pos_x = sp->pos[0];
                pos_y = sp->pos[1];
                dim_w = sp->dim[0];
                dim_h = sp->dim[1];
                better++;
            }
        }

        if(iter%logger_iter==0){
            int den = local_ag + local_ab + local_rb;
            std::cout<<"Iteration "<<iter<<": cost = "<<origin_e<<std::endl;
            std::cout<<"find betters: "<<better<<std::endl;
            std::cout<<"accept good rate:"<<(double)local_ag/den<<std::endl;
            std::cout<<"accept bad rate:"<<(double)local_ab/den<<std::endl;
            std::cout<<"reject bad rate:"<<(double)local_rb/den<<std::endl;
            std::cout<<"width:"<<b_width<<std::endl;
            std::cout<<"height:"<<b_height<<std::endl<<std::endl;;
            reject_rate = (double)local_rb/den;
            accept_good += local_ag;
            accept_bad += local_ab;
            reject_bad += local_rb;
            local_ag = 0;
            local_ab = 0;
            local_rb = 0;
            better = 0;
        }
        ++iter;
        cur_t *= m_descent_rate;
    }

    std::cout<<"accept good:"<<accept_good<<std::endl;
    std::cout<<"accept bad:"<<accept_bad<<std::endl;
    std::cout<<"reject bad:"<<reject_bad<<std::endl;
    std::cout<<"lowest energy:"<<b_origin_cost<<std::endl;
    std::cout<<"final area:"<<b_area<<std::endl;
    std::cout<<"final hpwl:"<<b_hpwl<<std::endl;
}

void SA::writeResult(const std::string& file_name){
    std::ofstream fout{file_name};
    if(!fout){
        std::cerr << "Error: can not open file"<<file_name<<std::endl;
        exit(1);
    }

    fout << (int)b_origin_cost <<std::endl;
    fout << b_hpwl << std::endl;
    fout << b_area << std::endl;
    fout << b_width << " "<< b_height << std::endl;
    fout << 0.87 << std::endl;
    std::vector<std::string>name;
    sp->nameList(name);
    for(int i=0;i<(int)name.size();++i){
        fout << name[i] << " " <<  pos_x[i] << " " << pos_y[i] << " " << pos_x[i]+dim_w[i] << " " << pos_y[i]+dim_h[i] <<std::endl;
    }

    fout.close();
}
