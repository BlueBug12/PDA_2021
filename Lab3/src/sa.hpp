#ifndef SA_HPP
#define SA_HPP
#define DISPLAY
//#define VERBOSE
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <fstream>
#include <float.h>
#include "sp.hpp"

class SA{
public:

    SA(double descent_rate, double initial_t, double final_t, int markov_iter):m_descent_rate(descent_rate),
    m_initial_t(initial_t),
    m_final_t(final_t),
    m_markov_iter(markov_iter){
        global_cost = DBL_MAX;
        global_hpwl = INT_MAX;
        global_area = INT_MAX;
        global_width = INT_MAX;
        global_height = INT_MAX;
        sp = nullptr;
        pass = false;
    }
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
    void updateResult();
    void writeResult(const std::string& file_name);
    
    SP *sp;
    std::vector<int>pos_x;
    std::vector<int>pos_y;
    std::vector<int>dim_w;
    std::vector<int>dim_h;
    double b_cost;
    double b_origin_cost;
    int b_hpwl;
    int b_area;
    int b_width;
    int b_height;
    bool pass;
    double global_cost;
    int global_hpwl;
    int global_area;
    int global_width;
    int global_height;

private:
    const double m_descent_rate;
    const double m_initial_t;
    const double m_final_t;
    const int m_markov_iter;
    std::vector<int> global_pos_x;
    std::vector<int> global_pos_y;
    std::vector<int> global_dim_w;
    std::vector<int> global_dim_h;
};

#endif
