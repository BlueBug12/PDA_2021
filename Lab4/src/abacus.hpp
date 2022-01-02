#ifndef ABACUS_HPP
#define ABACUS_HPP

#define F(n) for(int i=0;i<n;++i)

//#define DEBUG
#ifdef DEBUG
#   define M_Assert(Expr, Msg)  __M_Assert(#Expr, Expr, __FILE__, __FUNCTION__, __LINE__, Msg)
#else
#   define M_Assert(Expr, Msg) ;
#endif


#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <numeric>
#include <climits>
#include <cmath>
#include <map>
#include <algorithm>
#include <omp.h>
//#include <pthread.h>
#include <assert.h>
#include "data_struct.hpp"

class Abacus{
public:
    Abacus() = delete;
    Abacus(const std::string aux_file, int thread_num);
    Abacus(Abacus const &) = delete;
    Abacus(Abacus &&) = delete;
    Abacus & operator=(Abacus const &) = delete;
    Abacus & operator=(Abacus &&) = delete;
    ~Abacus() = default;
    
    void parser(const std::string& aux_file);
    void plParser(const std::string& pl_file);
    void sclParser(const std::string& scl_file, std::vector<std::pair<int,int>>& row_range);
    void nodesParser(const std::string& nodes_file);
    void genRows(std::vector<std::pair<int,int>>& row_range);//split row by terminals
    int searchRow(int cell_id);
    void run();
    void addCell(Cluster & c, int cell_id, int row_id);//may need to meet the constraint
    void addCluster(Cluster & c1, Cluster & c2);
    void collapse(const int x_min, const int x_max, std::vector<Cluster>& clusters, int & cost);
    void writeOutput(const std::string file_name);
    int placeRow(int cell_id, int row_id, bool recover = true);//return cost
    void writeGDT(std::string file_name);
    void getPosition();
    
    std::vector<Row>rows;
	std::vector<std::pair<int,int>>row_index;
    //after index [m_num_nodes-m_num_terminals], it represents information of terminals
    std::vector<int>width;
    std::vector<int>height;
    std::vector<int>x_coord;
    std::vector<int>y_coord;
    std::vector<int>ori_x;
    std::vector<int>ori_y;
    std::vector<int>order;
    std::vector<std::string>cell_names;

private:
    int m_num_rows;
    int m_cell_height;
    int m_row_base_height;
    int m_num_nodes;
    int m_num_terminals;    
    int m_threads;
    long long int m_total_cost;
};

#endif
