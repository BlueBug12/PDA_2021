#ifndef SA_HPP
#define SA_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <fstream>
#include "sp.hpp"

class SA{
public:

    SA(double descent_rate, double initial_t, double final_t, double scale, int markov_iter, double scale_descent_rate):m_descent_rate(descent_rate),
    m_initial_t(initial_t),
    m_final_t(final_t),
    m_scale(scale),
    m_markov_iter(markov_iter),
    m_scale_descent_rate(scale_descent_rate){}
    SA() = delete;
    ~SA(){
        delete sp;
    }
    SA(SA const &) = delete;
    SA(SA &&) = delete;
    SA & operator=(SA const &) = delete;
    SA & operator=(SA &&) = delete;
    

    void buildSP(const std::string block_name, const std::string net_name, double alpha);
    double acceptance(double old_e, double new_e, double temperature);
    void run();
    void writeResult(const std::string& file_name);
    
    SP *sp;
    std::vector<int>pos_x;
    std::vector<int>pos_y;
    std::vector<int>dim_w;
    std::vector<int>dim_h;
    std::vector<int>width;
    std::vector<int>height;
    double b_cost;
    int b_hpwl;
    int b_area;
    int b_width;
    int b_height;

private:
    const double m_descent_rate;
    const double m_initial_t;
    const double m_final_t;
    double m_scale;
    const int m_markov_iter;
    double m_scale_descent_rate;
};

#endif
