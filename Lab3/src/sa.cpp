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
    double cur_e = sp->getCost();
    double best_e = cur_e;

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
            sp->op2();
            double new_e = sp->getCost();
            if(new_e < cur_e){
                cur_e = new_e;
                accept_good += 1;
            }else{
                double prob = acceptance(cur_e,new_e,cur_t);
				fout<<cur_e<<" "<<new_e<<std::endl;
                if(prob > distr(eng)){
                    cur_e = new_e;
                    accept_bad += 1;
                }else{
                    reject_bad += 1;
                }
            }

            if(best_e > cur_e){
                best_e = cur_e;
                m_scale *= m_scale_descent_rate;
                pos_loci = sp->loci[0];
                neg_loci = sp->loci[1];
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
    std::cout<<"lowest energy:"<<best_e<<std::endl;
	fout.close();
}
