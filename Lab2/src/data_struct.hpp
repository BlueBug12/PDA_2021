#ifndef DATA_STRUCT_HPP 
#define DATA_STRUCT_HPP

#include <vector>
struct Net;
struct Cell{
    Cell(int id, bool l): cell_id(id),left(l){
        init_left = left;
        ret_left = left;
        lock = false;
        gain = 0;
    }
    Cell() = delete;
    Cell(Cell const &) = default;
    Cell(Cell      &&) = default;
    Cell & operator=(Cell const &) = default;
    Cell & operator=(Cell      &&) = default;
    ~Cell() = default;
    
    std::vector<Net*>nets;//store net id
    int cell_id;
    bool lock;
    bool left;//represent current group
    bool init_left;//set when each pass restart
    bool ret_left;//represent solution
    int gain;
};

struct Net{
    Net(int id): net_id(id){
        l_cells = 0;
        r_cells = 0;
        init_l_cells = 0;
        init_r_cells = 0;
    }
    Net() = delete;
    Net(Net const &) = default;
    Net(Net      &&) = default;
    Net & operator=(Net const &) = default;
    Net & operator=(Net      &&) = default;
    ~Net() = default;
    
    
    std::vector<Cell*>cells;//or unordered_set
    int net_id;
    int l_cells;
    int r_cells;
    int init_l_cells;
    int init_r_cells;
};


struct Record{
    Record(Cell* c, int gain, int sum, int cut, float balance):moved_cell(c), best_gain(gain),gain_sum(sum), cut_size(cut), balance_ratio(balance){}
    Record(int cut, float balance):cut_size(cut), balance_ratio(balance){
        moved_cell = NULL;
        best_gain = 0;
        gain_sum = 0;
#ifdef DEBUG
        if(balance_ratio>0.5)
            throw std::runtime_error("Error: invalid balance ratio.");
#endif
    }
    Record() = delete;
    Record(Record const &) = default;
    Record(Record      &&) = default;
    Record & operator=(Record const &) = default;
    Record & operator=(Record      &&) = default;
    ~Record() = default;
    
    Cell* moved_cell;
    int best_gain;
    int gain_sum;
    int cut_size;
    float balance_ratio;
};

#endif
