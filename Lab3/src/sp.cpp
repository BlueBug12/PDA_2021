#include "sp.hpp"

SP::SP(const std::string block_name, const std::string net_name, double alpha_):alpha(alpha_){
    parser(block_name,net_name);
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

    for(int i = 0 ;i<2;++i){
        match[i].resize(block_num);
        loci[i].resize(block_num);
        dim[i].resize(block_num);
        pos[i].resize(pin_num);
    }

    //initialize sequence pair
    for(int i=0;i<block_num;++i){
        loci[0][i] = loci[1][i] = i;
        match[0][i] = match[1][i] = i;
    }
    getArea();//initialize pos vector

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

    bound[0].resize(net_num,INT_MAX);//initialize left bound
    bound[1].resize(net_num,INT_MIN);//initialize right bound
    bound[2].resize(net_num,INT_MIN);//initialize up bound
    bound[3].resize(net_num,INT_MAX);//initialize down bound

    int degree;
    for(int i=0;i<net_num;++i){
        n_fin >> str >> degree;
        std::vector<int>net;
        for(int j=0;j<degree;++j){
            n_fin >> str;
            int id = index_map[str];
            updateBound(i,id);            
            net.push_back(id);       
        }
        nets.push_back(std::move(net));
    }
    n_fin.close();
}

void SP::updateNet(){
    bound[0].resize(net_num,INT_MAX);//initialize left bound
    bound[1].resize(net_num,INT_MIN);//initialize right bound
    bound[2].resize(net_num,INT_MIN);//initialize up bound
    bound[3].resize(net_num,INT_MAX);//initialize down bound

    for(int i=0;i<net_num;++i){
        for(int j=0;j<(int)nets[i].size();++j){
            updateBound(i,j);
        }
    }
}

void SP::updateBound(int net_id, int pos_id){
#ifdef DEBUG
    assert(net_id<net_num);
    assert(pos_id<block_num+terminal_num);
#endif 
    int x = pos[0][pos_id];
    int y = pos[1][pos_id];
    if(pos_id < block_num){
        x += dim[0][pos_id]/2;
        y += dim[1][pos_id]/2;
    }

    bound[0][net_id] = std::min(bound[0][net_id],x);//update left bound
    bound[1][net_id] = std::max(bound[1][net_id],x);//update right bound
    bound[2][net_id] = std::max(bound[2][net_id],y);//update up bound
    bound[3][net_id] = std::min(bound[3][net_id],y);//update down bound
}

int SP::getArea(){
    int width, height;
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
    width = lcs.back();
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
    height = lcs.front();
    return width*height;
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

int SP::getHPWL(){
    updateNet();
    int hpwl = 0;
    for(int i=0;i<net_num;++i){
#ifdef DEBUG
        assert(bound[0][i]!=INT_MAX);
        assert(bound[1][i]!=INT_MIN);
        assert(bound[2][i]!=INT_MIN);
        assert(bound[3][i]!=INT_MAX);
        assert(bound[0][i]<=bound[1][i]);
        assert(bound[2][i]>=bound[3][i]);
#endif
        hpwl += (bound[1][i]-bound[0][i]) + (bound[2][i]-bound[3][i]);
    }
    return hpwl;
}
/*
inline double SP::getCost(){
    return alpha*getArea() + (1-alpha)*getHPWL();
}*/

void SP::op1(){
    //std::srand(87);
    int x1 = std::rand()%block_num;
    int x2 = std::rand()%block_num;
    match[0][loci[0][x1]] = x2;
    match[0][loci[0][x2]] = x1;
    std::swap(loci[0][x1],loci[0][x2]);
}

void SP::op2(){
    /*
    for(int a=0;a<2;++a){
        for(int i=0;i<block_num;++i){
            std::cout<<loci[0][i]<<" ";
        }
        std::cout<<std::endl;
    }*/
    //std::srand(87);
    int b1 = std::rand()%block_num;
    int b2 = std::rand()%block_num;
    int pos_i1 = match[0][b1];
    int pos_i2 = match[0][b2];
    int neg_i1 = match[1][b1];
    int neg_i2 = match[1][b2];
    
    match[0][loci[0][pos_i1]] = pos_i2;
    match[0][loci[0][pos_i2]] = pos_i1;
    match[1][loci[1][neg_i1]] = neg_i2;
    match[1][loci[1][neg_i2]] = neg_i1;

   std::swap(loci[0][pos_i1],loci[0][pos_i2]);
   std::swap(loci[1][neg_i1],loci[1][neg_i2]);

   /*
    for(int a=0;a<2;++a){
        for(int i=0;i<block_num;++i){
            std::cout<<loci[a][i]<<" ";
        }
        std::cout<<std::endl;
        for(int i=0;i<block_num;++i){
            std::cout<<match[a][i]<<" ";
        }
        std::cout<<std::endl;
        std::cout<<std::endl;
    }
    exit(1);*/
}
void SP::op3(){
    //std::srand(87);
    int i = std::rand()%block_num;
    std::swap(dim[0][i],dim[1][i]);
}
