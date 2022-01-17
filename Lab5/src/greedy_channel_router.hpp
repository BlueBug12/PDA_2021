#ifndef GREEDY_CHANNEL_ROUTER_HPP
#define GREEDY_CHANNEL_ROUTER_HPP

#define F(n) for(int i=0;i<n;++i)

#define DEBUG
#ifdef DEBUG
#   define M_Assert(Expr, Msg)  __M_Assert(#Expr, Expr, __FILE__, __FUNCTION__, __LINE__, Msg)
#else
#   define M_Assert(Expr, Msg) ;
#endif

#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <assert.h>
#include "data_struct.hpp"

class GreedyCR{
public:
    GreedyCR() = delete;
    GreedyCR(const std::string filename);
    GreedyCR(const std::string filename,int icw, int mjl, int snc);
    GreedyCR(GreedyCR const &) = delete;
    GreedyCR(GreedyCR &&) = delete;
    GreedyCR & operator=(GreedyCR const &) = delete;
    GreedyCR & operator=(GreedyCR &&) = delete;
    ~GreedyCR() = default;
    void initialize(); 
    void parser(const std::string & filename);
    void run();
    void stepA(int cur_col);
    void stepB(int cur_col);
    void stepC(int cur_col);
    void stepD(int cur_col);
    void stepE(int cur_col);
    void stepF(int cur_col);

    void writeOutput(const std::string & filename);
    void writeGDT(const std::string & filename);

    int channel_width;
    int column_number;
    int net_number;
    Seg *dummy;
    
private:
    const int m_initial_channel_width;
    const int m_minimum_jog_length;
    const int m_steady_net_constant;
    
    std::list<Seg *>::iterator beg_it;
    std::list<Seg *>::iterator end_it;
    std::vector<int>pins[2];
    std::list<std::vector<Seg *>>tracks;
    std::vector<std::vector<Jog *>>columns;
    std::list<Seg *>frontier;
    std::vector<Net>nets;
    std::map<int,int>dict;
    std::vector<int>reverse;
};

#endif
