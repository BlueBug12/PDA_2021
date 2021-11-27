#include "sp.hpp"

SP::SP(const std::string block_name, const std::string net_name){
    parser(block_name,net_name);

    //initialize sequence pair
    for(int i = 0 ;i<2;++i){
        pos[i].resize(block_num);
        match[i].resize(block_num);
        loci[i].resize(block_num);
    }
    for(int i=0;i<block_num;++i){
        loci[0][i] = loci[1][i] = i;
        match[0][i] = match[1][i] = i;
    }
}
void SP::setInitial(const std::vector<int>& pos_loci, const std::vector<int>& neg_loci){
#ifdef DEBUG
    assert((int)pos_loci.size()==block_num && (int)neg_loci.size()==block_num);
#endif
    for(int i = 0;i<block_num;++i){
        loci[0][i] = pos_loci[i];
        loci[1][i] = neg_loci[i];
        match[0][pos_loci[i]] = i;
        match[1][neg_loci[i]] = i;
    }
}

void SP::parser(const std::string& block_name, const std::string& net_name){
    std::ifstream b_fin{block_name};
    if(!b_fin){
        std::cerr << "Error: can not open file"<<block_name<<std::endl;
        exit(1);
    }
    std::string str;
    b_fin >> str >> outline_w >> outline_h;
    b_fin >> str >> block_num;
    b_fin >> str >> terminal_num;
    
    int pin_num = block_num + terminal_num;

    dim[0].resize(block_num);
    dim[1].resize(block_num);
    pos[0].resize(pin_num);
    pos[1].resize(pin_num);   

    int w,h;
    for(int i=0;i<block_num;++i){
        b_fin >> str;
        index_map[str] = i;
        b_fin >> w >> h;
        dim[0][i] = w;
        dim[1][i] = h;
    }
    int x,y;
    for(int i=block_num;i<pin_num;++i){
        b_fin >> str;
        index_map[str] = i;
        b_fin >> str >> x >> y;
        pos[0][i] = x;
        pos[1][i] = y;
    }
    b_fin.close();

    std::ifstream n_fin{net_name};
    if(!n_fin){
        std::cerr << "Error: can not open file" << net_name <<std::endl;
        exit(1);
    }

    n_fin >> str >> net_num;
    int degree;
    for(int i=0;i<net_num;++i){
        n_fin >> str >> degree;
        std::vector<int>net;
        for(int j=0;j<degree;++j){
            n_fin >> str;
            net.push_back(index_map[str]);       
        }
        nets.push_back(std::move(net));
    }
    n_fin.close();
}

void SP::LCS(){
    std::vector<int>lcs;
    lcs.resize(block_num,0);
    //calculate x coordinate
    for(int i=0;i<block_num;++i){
        int id = loci[0][i];
        int p = match[1][id];
        pos[0][id] = lcs[p];
        int t =  pos[0][id] + dim[0][id];
        for(int j = p;j<block_num;++j){
            if(t>lcs[j]){
                lcs[j] = t;
            }else{
                break;
            }
        }
    }
    std::fill(lcs.begin(),lcs.end(),0);

    //calculate y coordinate
    for(int i=0;i<block_num;++i){
        int id = loci[1][i];
        int p = match[0][id];
        pos[1][id] = lcs[p];
        int t =  pos[1][id] + dim[1][id];
        for(int j = p;j>=0;--j){
            if(t>lcs[j]){
                lcs[j] = t;
            }else{
                break;
            }
        }
    }
    /*
    for(int i=0;i<block_num;++i){
        std::cout<<pos[0][i]<<" ";
    }
    std::cout<<std::endl;
    for(int i=0;i<block_num;++i){
        std::cout<<pos[1][i]<<" ";
    }
    std::cout<<std::endl;
    */
}
