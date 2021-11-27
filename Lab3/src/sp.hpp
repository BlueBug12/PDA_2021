#ifndef SP_HPP
#define SA_HPP
#define DEBUG

#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <assert.h>

class SP{
public:
    SP(const std::string block_name, const std::string net_name);
    SP() = delete;
    SP(SP const & ) = delete;
    SP(SP       &&) = delete;
    SP & operator=(SP const &) = delete;
    SP & operator=(SP      &&) = delete;
    ~SP() = default;
    void parser(const std::string& block_name, const std::string& net_name);
    void setInitial(const std::vector<int>& pos_loci, const std::vector<int>& neg_loci);
    void LCS();
    int getArea();
    int getHPWL();
    float cost();

private:
    int block_num;
    int terminal_num;
    int net_num;
    int outline_w;
    int outline_h;
    std::unordered_map<std::string,int>index_map;
    std::vector<int>dim[2];// 0 for width, 1 for height
    std::vector<int>pos[2];
    std::vector<std::vector<int>> nets;

    std::vector<int>loci[2];
    std::vector<int>match[2];
};

#endif
