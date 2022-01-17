#ifndef DATA_STRUCT_HPP
#define DATA_STRUCT_HPP

#include <vector>

struct Seg;
struct Jog{
    Jog(int id, int col_id):net_id(id){
        beg = nullptr;
        end = nullptr;
        col = col_id;
    }
    Jog(int id, int col_id, Seg *beg_ptr, Seg *end_ptr):net_id(id){
        assert(beg_ptr!=end_ptr);
        beg = beg_ptr;
        end = end_ptr;
        col = col_id;
    }
    Jog(Jog const &) = default;
    Jog(Jog &&) = default;
    Jog & operator=(Jog const &) = default;
    Jog & operator=(Jog &&) = default;
    ~Jog() = default;
    
    const int net_id;
    int col;
    Seg * beg;
    Seg * end;
};

struct Seg{
    Seg(int id):net_id(id){
        beg = -1;
        end = -1;
    }
    Seg(int id, int beg_id):net_id(id){
        beg = beg_id;
        end = beg_id;
    }
    Seg(int id, int beg_id, int end_id):net_id(id){
        beg = beg_id;
        end = end_id;
    }
    Seg(Seg const &) = default;
    Seg(Seg &&) = default;
    Seg & operator=(Seg const &) = default;
    Seg & operator=(Seg &&) = default;
    ~Seg() = default;
    
    const int net_id;
    bool close = false;
    int beg;
    int end;
    int track_id = -1;
};

struct Net{
    Net() = default;
    Net(Net const &) = default;
    Net(Net &&) = default;
    Net & operator=(Net const &) = default;
    Net & operator=(Net &&) = default;
    ~Net(){
        for(size_t i=0;i<jogs.size();++i){
            delete jogs.at(i);
        }
        for(size_t i=0;i<segments.size();++i){
            delete segments.at(i);
        }
    }
    
    int counter = 0;
    std::vector<Jog *>jogs;
    std::vector<Seg *>segments;
    std::vector<int>pin_pos[2];
    int cur_pos[2] = {-1,-1};//{top,bot}
};

#endif
