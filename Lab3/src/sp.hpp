#ifndef SP_HPP
#define SA_HPP
#define DEBUG

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <climits>
#include <assert.h>

class SP{
public:
    SP(const std::string block_name, const std::string net_name, double alpha_);
    SP() = delete;
    SP(SP const & ) = delete;
    SP(SP       &&) = delete;
    SP & operator=(SP const &) = delete;
    SP & operator=(SP      &&) = delete;
    ~SP() = default;
    void parser(const std::string& block_name, const std::string& net_name);
    void setInitial();
    void updateBound(int net_id, int pos_id);
    void updateNet();
    int getArea(int& width, int& height);
    int getHPWL();
    inline double getCost(int& w, int& h, int& hpwl, int& area){
        int w_,h_;
        area = getArea(w_,h_);
        hpwl = getHPWL();
        w = w_;
        h = h_;
        return alpha*area + (1-alpha)*hpwl;
    }
    void op1();
    void op2();
    void op3();
    void nameList(std::vector<std::string>& name);

    std::vector<int>loci[2];
    std::vector<int>dim[2];// 0 for width, 1 for height
    std::vector<int>pos[2];

    int block_num;
    int terminal_num;
    int net_num;
    int outline_w;
    int outline_h;


private:
    const double alpha;
    std::unordered_map<std::string,int>index_map;
    std::vector<std::vector<int>> nets;
    std::vector<int>bound[4];//left,right,up,down

    std::vector<int>match[2];
};

#endif
