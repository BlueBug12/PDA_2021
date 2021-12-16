#ifndef DATA_STRUCT_HPP
#define DATA_STRUCT_HPP

struct Cluster{
    Cluster(){
        x = 0;
        e = 0;
        q = 0;
        w = 0;
        beg = -1;
        end = -1;
    }
    Cluster(Cluster const &) = delete;
    Cluster(Cluster &&) = delete;
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
    Row(int x1, int x2, int _y):left_x(x1),right_x(x2),y(_y){}
    Row(Row const &) = delete;
    Row(Row &&) = delete;
    Row & operator=(Row const &) = default;
    Row & operator=(Row &&) = default;
    ~Row() = default;
    const int left_x;
    const int right_x;
    const int y;
    std::vector<int>cells;
    std::vector<Cluster *>clusters;//??
};

#endif
