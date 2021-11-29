#include "sa.hpp"

void SA::buildSP(const std::string block_name, const std::string net_name, double alpha){
    sp = new SP(block_name,net_name,alpha);
}


double SA::acceptance(double old_e, double new_e, double temperature){
    return std::exp((old_e - new_e)/temperature); 
}

void SA::run(){
	std::ofstream fout{"log.txt"};

    double cur_t = m_initial_t;
    int cur_w, cur_h, cur_hpwl, cur_area;
    double cur_e = sp->getCost(cur_w, cur_h, cur_hpwl, cur_area);
    b_cost = cur_e;
    b_width = cur_w;
    b_height = cur_h;
    b_hpwl = cur_hpwl;
    b_area = cur_area;
    std::cout<<"initial hpwl:"<<b_hpwl<<std::endl;
    std::cout<<"initial area:"<<b_area<<std::endl;
    std::cout<<"initial cost:"<<b_cost<<std::endl;

    pos_x = sp->pos[0];
    pos_y = sp->pos[1];
    dim_w = sp->dim[0];
    dim_h = sp->dim[1];

    int accept_good = 0;
    int accept_bad = 0;
    int reject_bad = 0;
    int iter = 0;
    int logger_iter = 100;
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<double> distr(0, 1);

    while(cur_t >= m_final_t){
        for(int i=0;i<m_markov_iter;++i){
            sp->op1();
			double new_e = sp->getCost(cur_w, cur_h, cur_hpwl, cur_area);
			fout<<cur_e<<" "<<new_e <<std::endl;
            for(int i=0;i<pos_x.size();++i){
                fout<<sp->loci[0][i]<<" ";
            }
            fout<<std::endl;
            if(new_e < cur_e){
                cur_e = new_e;
                accept_good += 1;
            }else{
                double prob = acceptance(cur_e,new_e,cur_t);
                if(prob > distr(eng)){
                    cur_e = new_e;
                    accept_bad += 1;
                }else{
                    reject_bad += 1;
                }
            }

            if(b_cost > cur_e){
                b_cost = cur_e;
                b_width = cur_w;
                b_height = cur_h;
                b_hpwl = cur_hpwl;
                b_area = cur_area;
                m_scale *= m_scale_descent_rate;
                pos_x = sp->pos[0];
                pos_y = sp->pos[1];
                dim_w = sp->dim[0];
                dim_h = sp->dim[1];
            }
        }
        if(iter%logger_iter==0){
            std::cout<<"Iteration "<<iter<<": cost = "<<cur_e<<std::endl;
        }
        ++iter;
        cur_t *= m_descent_rate;
    }

    std::cout<<"accept good:"<<accept_good<<std::endl;
    std::cout<<"accept bad:"<<accept_bad<<std::endl;
    std::cout<<"reject bad:"<<reject_bad<<std::endl;
    std::cout<<"lowest energy:"<<b_cost<<std::endl;
    std::cout<<"final area:"<<b_area<<std::endl;
    std::cout<<"final hpwl:"<<b_hpwl<<std::endl;
	fout.close();
}

void SA::writeResult(const std::string& file_name){
    std::ofstream fout{file_name};
    if(!fout){
        std::cerr << "Error: can not open file"<<file_name<<std::endl;
        exit(1);
    }

    fout << (int)b_cost <<std::endl;
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
