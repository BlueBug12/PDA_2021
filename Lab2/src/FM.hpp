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
    std::vector<Net*>all_nets;
    std::unordered_map<int, Cell*>all_cells;
    std::vector<std::pair<int,Cell*>>ordered_cells;
    bool stop;
    int min_cut;
    float best_balance;
    int case1;
    int case2;
    int case3;
    int case4;
private:
    std::vector<std::list<Cell*>>_l_group;
    std::vector<std::list<Cell*>>_r_group;
    std::vector<Record>recorder;
    int _left_num;
    int _right_num;
    int _shift;// = Pmax (for the bucket )
};

#endif
