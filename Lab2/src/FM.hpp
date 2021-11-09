#ifndef FM_HPP 
#define FM_HPP
//#define DEBUG
//#define PRINTER 

#include <vector>
#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <map>
#include <set>
#include <assert.h>

class FM{
public:
    FM(const std::string & file_name, float ratio);
    FM()=delete;
    FM(FM const &) = delete;
    FM(FM      &&) = delete;
    FM & operator=(FM const &) = delete;
    FM & operator=(FM      &&) = delete;
    ~FM()=default;
    
    void readInput(const std::string & file_name);
    void writeOutput(const std::string & file_name);
    void initialize();//distribute cells into two groups and set the initial F T values
    void updateGain(size_t cell_id);//update net gain & cell gain
    size_t chooseCell();//choose the valid candidate with largest gain 
    inline int getCutSize();
    int calGain(size_t cell_id);
    void undoGroup();
    void run();

    size_t cell_num;
    size_t net_num;
    size_t min_group;
    size_t max_group;

    int min_cut;
    int current_cut;

    std::vector<std::vector<size_t>> net_list;//store the cell list for each net
    std::vector<std::vector<size_t>> cell_list;//store the net list for each cell

    std::vector<int>cell_num_in_net[2];
    std::vector<bool>group;
    std::vector<bool>answer;
    std::vector<int>cell_gain;
    std::map<int,size_t>key;//real id : pseudo id 
    std::set<std::pair<int,size_t>,std::greater<std::pair<int,int>>>bucket[2];
    int cell_counter[2];
};

#endif
