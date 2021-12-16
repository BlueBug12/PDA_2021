#ifndef ABACUS_HPP
#define ABACUS_HPP

#define F(n) for(int i=0;i<n;++i)
#define DBG(msg, arg...) printf("%s:%s(%d): " msg, __FILE__, __FUNCTION__, __LINE__, ##arg)

#define DEBUG

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <assert.h>
#include "data_struct.hpp"

class Abacus{
public:
    Abacus() = delete;
    Abacus(const std::string aux_file);
    Abacus(Abacus const &) = delete;
    Abacus(Abacus &&) = delete;
    Abacus & operator=(Abacus const &) = delete;
    Abacus & operator=(Abacus &&) = delete;
    ~Abacus() = default;
    
    void parser(const std::string& aux_file);
    void plParser(const std::string& pl_file);
    void sclParser(const std::string& scl_file);
    void nodesParser(const std::string& nodes_file);
    void genRows();//split row by terminals
    void searchRow();
    void run();
    void addCell(Cluster * c, int cell_id, int pos);//may need to meet the constraint
    void addCluster(Cluster * c1, Cluster * c2);
    Cluster * collapse(Cluster * c);
    void writeOutput();
    int placeRow(std::vector<Cluster *>record, int r_index);//return cost
    void getPosition();
    
    std::vector<Row>rows;
    //after index [m_num_nodes-m_num_terminals], it represents information of terminals
    std::vector<int>width;
    std::vector<int>height;
    std::vector<int>x_coord;
    std::vector<int>y_coord;
    std::vector<int>order;

private:
    int m_num_rows;
    int m_cell_height;
    int m_num_nodes;
    int m_num_terminals;    
};

#endif