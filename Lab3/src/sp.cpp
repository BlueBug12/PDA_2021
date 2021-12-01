#include "sp.hpp"

SP::SP(const std::string block_name, const std::string net_name, double alpha_):alpha(alpha_){
    bounded_alpha = std::max(0.1,alpha);
    parser(block_name,net_name);
}
void SP::setInitial(){
    std::random_device rd;
    std::mt19937 eng(rd());
    for(int i=0;i<2;++i){
        for(int j = 0;j<block_num;++j){
            loci[i][j] = j;
        }
    }
    for(int i = 0;i<2;++i){
        std::shuffle(loci[i].begin(),loci[i].end(),eng);
        for(int j=0;j<block_num;++j){
            match[i][loci[i][j]] = j;
        }
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
        dim[i].resize(block_num);
        loci[i].resize(block_num);
        pos[i].resize(pin_num);
    }

    //initialize sequence pair
    setInitial();

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

    getArea(w,h);//initialize pos vector

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
            net.push_back(id);       
        }
        nets.push_back(std::move(net));
    }
    n_fin.close();
}

void SP::updateNet(){
    std::fill(bound[0].begin(),bound[0].end(),INT_MAX);
    std::fill(bound[1].begin(),bound[1].end(),INT_MIN);
    std::fill(bound[2].begin(),bound[2].end(),INT_MIN);
    std::fill(bound[3].begin(),bound[3].end(),INT_MAX);
    for(int i=0;i<net_num;++i){
        for(int j=0;j<(int)nets[i].size();++j){
            updateBound(i,nets[i][j]);
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

double SP::skew(int w, int h){
    double r = (double)(w*outline_h) / (double)(h*outline_w);
    if(r < 1)
        r = 1/r;
    if(w>outline_w){
        r*=1.1;
    }
    if(h>outline_h){
        r*=1.1;
    }
    if(w<=outline_w && h <= outline_h){
        r*=0.8;
    }
    return r;
}
double SP::getCost(int& w, int& h, int& hpwl, int& area, int& origin_cost){
    int w_,h_;
    area = getArea(w_,h_);
    hpwl = getHPWL();
    w = w_;
    h = h_;
    origin_cost = alpha*area + (1-alpha)*hpwl;
    return skew(w,h)*bounded_alpha*(double)area + (1-bounded_alpha)*(double)hpwl;
}

int SP::getArea(int & width, int & height){
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
        int p = block_num -1 - match[0][id];
        pos[1][id] = lcs[p];
        int t =  pos[1][id] + dim[1][id];
        for(int j = p;j<block_num;++j){
            if(t>lcs[j]){
                lcs[j] = t;
            }else{
                break;
            }
        }
    }
    height = lcs.back();
    return width*height;
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

void SP::op1(){
    int x1 = std::rand()%block_num;
    int x2 = std::rand()%block_num;
    while(x2==x1){
        x2 = std::rand()%block_num;
    }
    int index = std::rand()%2;
    std::swap(match[index][loci[index][x1]],match[index][loci[index][x2]]);
    std::swap(loci[index][x1],loci[index][x2]);
    op_record[0] = x1;
    op_record[1] = x2;
    op_record[2] = index;
    option = 1;
}

void SP::op2(){
    int b1 = std::rand()%block_num;
    int b2 = std::rand()%block_num;
    while(b2==b1){
        b2 = std::rand()%block_num;
    }
    int pos_i1 = match[0][b1];
    int pos_i2 = match[0][b2];
    int neg_i1 = match[1][b1];
    int neg_i2 = match[1][b2];
    
    std::swap(match[0][b1],match[0][b2]);
    std::swap(match[1][b1],match[1][b2]);

   std::swap(loci[0][pos_i1],loci[0][pos_i2]);
   std::swap(loci[1][neg_i1],loci[1][neg_i2]);
   op_record[0] = b1;
   op_record[1] = b2;
   option = 2;
}
void SP::op3(){
    int i = std::rand()%block_num;
    std::swap(dim[0][i],dim[1][i]);
    op_record[0] = i;
    option = 3;
}

void SP::nameList(std::vector<std::string>&name){
    name.resize(block_num);
    for(auto p: index_map){
        if(p.second<block_num)
            name[p.second] = p.first;
    }
}

void SP::reverse(){
    //loci dim match
    if(option==1){
        int x1 = op_record[0];   
        int x2 = op_record[1];
        int index = op_record[2];
        std::swap(loci[index][x1],loci[index][x2]);
        std::swap(match[index][loci[index][x1]],match[index][loci[index][x2]]);
    }else if(option==2){
        int b1 = op_record[0]; 
        int b2 = op_record[1];
        int pos_i1 = match[0][b1];
        int pos_i2 = match[0][b2];
        int neg_i1 = match[1][b1];
        int neg_i2 = match[1][b2];

        std::swap(match[0][b1],match[0][b2]);
        std::swap(match[1][b1],match[1][b2]);
        std::swap(loci[0][pos_i1],loci[0][pos_i2]);
        std::swap(loci[1][neg_i1],loci[1][neg_i2]);

    }else if(option==3){
        std::swap(dim[0][op_record[0]],dim[1][op_record[0]]);
    }else{
        std::cerr<<"Error: unexcepted option."<<std::endl;
    }
}
