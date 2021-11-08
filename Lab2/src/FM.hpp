#ifndef FM_HPP 
#define FM_HPP
//#define DEBUG
//#define PRINTER 

#include <vector>
#include <unordered_map>
#include <climits>
#include <list>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <ctime>
#include "data_struct.hpp"

class FM{
public:
    FM(const std::string & file_name, float ratio);
    FM()=delete;
    FM(FM const &) = delete;
    FM(FM      &&) = delete;
    FM & operator=(FM const &) = delete;
    FM & operator=(FM      &&) = delete;
    ~FM();
    
    void readInput(const std::string & file_name);
    void writeOutput(const std::string & file_name);
    void initialize();//distribute cells into two groups and set the initial F T values
    void updateGain(Cell* target);//update net gain & cell gain
    Cell* chooseCell();//choose the valid candidate with largest gain 
    inline bool checkBalance(Cell* c);//check the balance when we want to move c to another group
    void unlockAll();
    Cell* findTarget(const int net_id, const bool left);//find the target cell in the specified net (for the case that T(N)=1 or F(N)=1)
    void storeResult();
    inline int getCutSize();
    void resetGain();
    
    size_t cell_num;
    size_t net_num;
    int min_group;
    int max_group;
    //std::vector<Net*>all_nets;
    //std::vector<std::pair<int,Cell*>>ordered_cells;
    bool stop;
    int min_cut;
    float best_balance;

    std::vector<std::vector<int>> netlist;
    std::vector<int> n_l_num;
    std::vector<int> n_r_num;
    std::vector<bool>lock;
    std::vector<bool>group;
    std::set<int> cell;
    std::map<int,int>key;//real id : pseudo id 
private:
    std::map<int,std::set<int>>m_l_group;
    std::vector<int,std::set<int>>m_r_group;
    
    int m_left_cell;
    int m_right_cell;
};

#endif
