#include "abacus.hpp"

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

    nodesParser(dir+nodes_file);
    sclParser(dir+scl_file);
    plParser(dir+pl_file);
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
        //x_coord.push_back(x);
        //y_coord.push_back(y);
    }
    fin.close();
}
void Abacus::sclParser(const std::string& scl_file){
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
    ss >> str >> m_num_rows;
    
    
    F(m_num_rows){
        int x, y, width,space;
        fin >> str >> str;//CoreRow Horizontal
        fin >> str >> str >> y;//Coordinate : 459
        fin >> str >> str >> m_cell_height;//Height : 12
        fin >> str >> str >> str;//Sitewidth : 1
        fin >> str >> str >> space;//Sitespacing : 1
        fin >> str >> str >> str;//Siteorient : 1
        fin >> str >> str >> str;//Sitesymmetry : 1
        fin >> str >> str >> x >> str >> str >> width;//SubrowOrigin : 459 Numsites : 10692
        fin >> str;//End
        width *= space;
    }

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
void Abacus::genRows(){}//split row by terminals
void Abacus::searchRow(){}
void Abacus::run(){}
void Abacus::addCell(Cluster * c, int cell_id, int pos){}//may need to meet the constraint
void Abacus::addCluster(Cluster * c1, Cluster * c2){}
Cluster * Abacus::collapse(Cluster * c){}
void Abacus::writeOutput(){}
int Abacus::placeRow(std::vector<Cluster *>record, int r_index){}//return cost
void Abacus::getPosition(){}
