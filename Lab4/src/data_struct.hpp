#ifndef DATA_STRUCT_HPP
#define DATA_STRUCT_HPP

struct Cluster{
    Cluster(int _x, int _beg):x(_x),beg(_beg){
        e = 0;
        q = 0;
        w = 0;
        end = -1;
    }
    Cluster(Cluster const &) = default;
    Cluster(Cluster &&) = default;
    Cluster & operator=(Cluster const &) = default;
    Cluster & operator=(Cluster &&) = default;
    ~Cluster() = default;
    
    int x;
    int e;
    int q;
    int w;
    int beg;
    int end;
};

struct Row{
    Row() = delete;
    Row(int x1, int x2, int _y):left_x(x1),right_x(x2),y(_y){
        space = right_x - left_x;
    }
    Row(Row const &) = delete;
    Row(Row &&) = default;
    Row & operator=(Row const &) = default;
    Row & operator=(Row &&) = default;
    ~Row() = default;
    const int left_x;
    const int right_x;
    const int y;
    int space;
    std::vector<int>cells;
    std::vector<Cluster>clusters;//??
};

#endif
