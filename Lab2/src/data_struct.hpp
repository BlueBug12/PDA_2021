#ifndef DATA_STRUCT_HPP 
#define DATA_STRUCT_HPP

#include <set>

struct Net{
    Net(int id): net_id(id){
        l_cells = 0;
        r_cells = 0;
    }
    Net() = delete;
    Net(Net const &) = default;
    Net(Net      &&) = default;
    Net & operator=(Net const &) = default;
    Net & operator=(Net      &&) = default;
    ~Net() = default;
    
    
    std::set<int>nodes;//or unordered_set
    int net_id;
    int l_cells;
    int r_cells;
};

struct Cell{
    Cell(int id, bool l): cell_id(id),left(l){
        lock = false;
        gain = 0;
    }
    Cell() = delete;
    Cell(Cell const &) = default;
    Cell(Cell      &&) = default;
    Cell & operator=(Cell const &) = default;
    Cell & operator=(Cell      &&) = default;
    ~Cell() = default;
    
    std::set<int>nets;//store net id
    int cell_id;
    bool lock;
    bool left;
    int gain;
};

struct Record{
    Record(Cell* c, int sum, int cut):moved_cell(c), best_gain(c->gain),gain_sum(sum), cut_size(cut){}
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
};

#endif
