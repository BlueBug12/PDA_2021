#ifndef _RPLPARSER_H_
#define _RPLPARSER_H_
#include <fstream>
#include <vector>
#include <string>
using std::string;
using std::vector;
class Dumper;
//----------------------------------------------------------------------------------------------------------------------//
struct rplNode_S
{
    char nodeName[32];
    int width;  
    int height;  
    int nodeType;
};
//----------------------------------------------------------------------------------------------------------------------//
struct rplNetNode_S
{
    string netNodeName;
    int netNodeIO;
    float netNodeX;
    float netNodeY;
};
//----------------------------------------------------------------------------------------------------------------------//
struct rplNet_S
{
    string netName;
    vector<rplNetNode_S*> vNetNode;
};
//----------------------------------------------------------------------------------------------------------------------//
struct rplPlNode_S
{
    string plNodeName;
    int plNodeX;
    int plNodeY;
    int plNodeFixed;
};
//----------------------------------------------------------------------------------------------------------------------//
struct rplRow_S
{
    int rowCoord;
    int rowHeight;
    int rowSiteWidth;
    int rowSiteSpace;
    char rowSiteOrient;
    char rowSiteSym;
    int rowSubRowOrig;
    int numSite;
};
//----------------------------------------------------------------------------------------------------------------------//
struct rplData_S
{
    std::vector<rplNode_S*> vNode;
    std::vector<rplNet_S*> vNet;
    std::vector<rplPlNode_S*> vPlNode;
    std::vector<rplRow_S*> vRow;
    int gridOrigX;
    int gridOrigY;
    int nMoveNode;
    int nFixNode;
    int nOverlapFixNode;
    int nChipWidth; 
    int nChipHeight; 
    char m_caInputAux[32];
};
//-----------------------------------------------------------------------------------------------//
class rplParser_C
{
    public:
        void dump(Dumper&);
        rplParser_C(const char*);
        ~rplParser_C();
        //Main API
        void parse();

        //Sub Function
        void parseAux();
        void parseNode();
        void parseNet();
        void parsePl();
        void parseScl();
        rplData_S* getRplData() {return m_pRplData;}
    private:
        void getLine(std::ifstream&, char*,int);
        char m_caInputAux[32];
        string m_caInputNode;
        string m_caInputNet;
        string m_caInputPl;
        string m_caInputScl;
        std::ifstream m_fInputAux;
        std::ifstream m_fInputNode;
        std::ifstream m_fInputNet;
        std::ifstream m_fInputPl;
        std::ifstream m_fInputScl;
        rplData_S* m_pRplData;
};
//-----------------------------------------------------------------------------------------------//


#endif
