#include "abacus.hpp"
void __M_Assert(const char* expr_str, bool expr, const char* file, const char* function, int line, const std::string msg)
{
    if (!expr)
    {
        std::cerr << "Assert failed:\t" << msg << "\n"
            << "Expected:\t" << expr_str << "\n"
            << "Source:\t\t" << file <<": "<< function << "()" << ", line " << line << "\n";
        abort();
    }
}

std::string intsToString(int v1,int v2){
	return std::to_string(v1) + "," + std::to_string(v2);
}

Abacus::Abacus(const std::string aux_file){
    parser(aux_file);
}
void Abacus::parser(const std::string& aux_file){
    std::ifstream fin{aux_file};
    if(!fin){
        std::cerr << "Error: can not open file "<<aux_file<<std::endl;
        exit(1);
    }
    std::string str;
    std::string nodes_file;
    std::string pl_file;
    std::string scl_file;

    fin >> str >> str >> nodes_file >> str >> str >> pl_file >> scl_file;
    fin.close();
    std::string dir;
    size_t index = aux_file.rfind("/");
    if(index!=std::string::npos){
        dir = aux_file.substr(0,index+1);
    }
    std::vector<std::pair<int,int>>row_range;
    nodesParser(dir+nodes_file);
    sclParser(dir+scl_file,row_range);
    plParser(dir+pl_file);

    genRows(row_range); 
}

void Abacus::plParser(const std::string& pl_file){
    std::ifstream fin{pl_file};
    if(!fin){
        std::cerr << "Error: can not open file "<<pl_file<<std::endl;
        exit(1);
    }
    std::string str;
    F(3){
        std::getline(fin,str);
    }
    F(m_num_nodes - m_num_terminals){
        int x,y;
        fin >> str >> x >> y >> str >> str;
        x_coord.push_back(x);
        y_coord.push_back(y);
    }

    F(m_num_terminals){
        int x,y;
        fin >> str >> x >> y >> str >> str >> str;
        x_coord.push_back(x);
        y_coord.push_back(y);
    }
    fin.close();
}
void Abacus::sclParser(const std::string& scl_file, std::vector<std::pair<int,int>>&row_range){
    std::ifstream fin{scl_file};
    if(!fin){
        std::cerr << "Error: can not open file "<<scl_file<<std::endl;
        exit(1);
    }
    std::string str;
    std::getline(fin,str);
    while(str.find("NumRows")==std::string::npos){
        std::getline(fin,str);
    }
    std::stringstream ss(str);
    ss >> str >> str >> m_num_rows;
    
    int x, width,space;
    fin >> str >> str;//CoreRow Horizontal
    fin >> str >> str >> m_row_base_height;//Coordinate : 459
    fin >> str >> str >> m_cell_height;//Height : 12
    fin >> str >> str >> str;//Sitewidth : 1
    fin >> str >> str >> space;//Sitespacing : 1
    fin >> str >> str >> str;//Siteorient : 1
    fin >> str >> str >> str;//Sitesymmetry : 1
    fin >> str >> str >> x >> str >> str >> width;//SubrowOrigin : 459 Numsites : 10692
    fin >> str;//End
    width *= space;
    row_range.push_back({x,x+width});
    //std::getline(fin,str);

    F(m_num_rows -1){
        for(int j=0;j<5;++j){
            std::getline(fin,str);
        }
        fin >> str >> str >> space;//Sitespacing : 1
        fin >> str >> str >> str;//Siteorient : 1
        fin >> str >> str >> str;//Sitesymmetry : 1
        fin >> str >> str >> x >> str >> str >> width;//SubrowOrigin : 459 Numsites : 10692
        fin >> str;//End
        width *= space;
        row_range.push_back({x,x+width});
    }
#ifdef DEBUG
    F(m_num_rows-1){
        assert(row_range[i].first==row_range[i+1].first);
        assert(row_range[i].second==row_range[i+1].second);
    }
#endif

    fin.close();
}
void Abacus::nodesParser(const std::string& nodes_file){
    std::ifstream fin{nodes_file};
    if(!fin){
        std::cerr << "Error: can not open file "<<nodes_file<<std::endl;
        exit(1);
    }
    std::string str;
    std::getline(fin,str);
    while(str.find("NumNodes")==std::string::npos){
        std::getline(fin,str);
    }
    std::stringstream ss(str);
    ss >> str >> str >> m_num_nodes;
    fin >> str >> str >> m_num_terminals;
    F(m_num_nodes - m_num_terminals){
        int x,y;
        fin >> str >> x >> y;
        width.push_back(x);
        height.push_back(y);
    }
    F(m_num_terminals){
        int x,y;
        fin >> str >> x >> y >> str;
        width.push_back(x);
        height.push_back(y);
    }
#ifdef DEBUG
    assert((int)width.size()==m_num_nodes);
    assert((int)height.size()==m_num_nodes);
#endif
    fin.close();
}
void Abacus::genRows(std::vector<std::pair<int,int>>& row_range){//split row by terminals
    std::vector<std::map<int,int>>spliter;
    spliter.resize(m_num_rows);
    for(int i = m_num_nodes-m_num_terminals;i<m_num_nodes;++i){
        int y1 = std::max(m_row_base_height,y_coord.at(i)) - m_row_base_height;
        int y2 = std::min(m_row_base_height + m_cell_height * m_num_rows, y_coord.at(i) + height.at(i)) - m_row_base_height;
        if(y1 >= y2)
            continue;
        int lower_bound = y1/m_cell_height;
        int upper_bound = y2/m_cell_height;
        if(y2 % m_cell_height==0){
            upper_bound -= 1;
        }
#ifdef DEBUG
        int lower = lower_bound*m_cell_height+m_row_base_height;
        int upper = upper_bound*m_cell_height+m_row_base_height + m_cell_height;
        int t1 = std::max(m_row_base_height,y_coord.at(i));
        int t2 = std::min(m_row_base_height + m_cell_height * m_num_rows, y_coord.at(i) + height.at(i));
        assert( lower <= t1 && lower+m_cell_height > t1);
        assert( upper >=  t2 && upper-m_cell_height < t2);
        assert(lower_bound>=0&&upper_bound<(int)spliter.size());
#endif
        auto prune = [](int val,int min_v,int max_v)->int {
            return std::max(std::min(val,max_v),min_v);
        };
        while(lower_bound<=upper_bound){
            int min_left = row_range.at(lower_bound).first;
            int max_right = row_range.at(lower_bound).second;
            if(x_coord.at(i)>=max_right||x_coord.at(i)+width.at(i)<=min_left){
                ++lower_bound;
                continue;
            }
            spliter.at(lower_bound)[prune(x_coord.at(i),min_left,max_right)]++;
            spliter.at(lower_bound)[prune(x_coord.at(i)+width.at(i),min_left,max_right)]--;
            ++lower_bound;
        }
    }
#ifdef DEBUG
    int start_index = (int)rows.size();
#endif
    F(m_num_rows){
        int beg = row_range.at(i).first;
        int end = row_range.at(i).second;
        int row_height = m_row_base_height+i*m_cell_height;
#ifdef DEBUG
        bool flag =true;
#endif
        for(auto & pair: spliter.at(i)){
#ifdef DEBUG
            assert(pair.second<=1&&pair.second>=-1);
            assert(beg<=pair.first);
            if(pair.second==1)
                assert(flag==true);
            else if(pair.second==-1)
                assert(flag==false);
            else
                flag = !flag;
            flag = !flag;
#endif
            if(pair.second == 1){//terminal left
                if(beg != pair.first){
                    rows.push_back(Row(beg,pair.first,row_height));
                }
            }else if(pair.second == -1){//terminal right
                beg = pair.first;
            }else{
                continue;
            }
        }
        if(beg < end){
            rows.push_back(Row(beg,end,row_height));
        }
#ifdef DEBUG
		for(;start_index<(int)rows.size();++start_index){
			Row& r = rows.at(start_index);
			assert(r.left_x<r.right_x);
			if(start_index<(int)rows.size()-1)
				M_Assert(r.right_x < rows.at(start_index+1).left_x,intsToString(r.right_x,rows.at(start_index+1).left_x));
		}
#endif
    }
}

int Abacus::searchRow(){}
void Abacus::run(){}
void Abacus::addCell(Cluster * c, int cell_id, int pos){}//may need to meet the constraint
void Abacus::addCluster(Cluster * c1, Cluster * c2){}
Cluster * Abacus::collapse(Cluster * c){}
void Abacus::writeOutput(){}
int Abacus::placeRow(std::vector<Cluster *>record, int r_index){}//return cost
void Abacus::getPosition(){}
