#include <cassert>
#include <cstring>
#include <climits>
#include <stdlib.h>
#include "message.h"
#include "rplParser.h"
using namespace std;
//----------------------------------------------------------------------------------------------------------------------//
rplParser_C::rplParser_C(const char* p_caInputAux)
{
    m_pRplData = new rplData_S;
    strcpy(m_caInputAux,p_caInputAux);
    strcpy(m_pRplData->m_caInputAux,m_caInputAux);
    m_caInputNode  = "";
    m_caInputNet   = "";
    m_caInputPl    = "";
    m_caInputScl   = "";
    
}
//----------------------------------------------------------------------------------------------------------------------//
rplParser_C::~rplParser_C()
{
    if(m_pRplData)
    {
        for(unsigned nNodeIter=0;nNodeIter<m_pRplData->vNode.size();++nNodeIter) delete m_pRplData->vNode[nNodeIter];
        for(unsigned nNetIter=0;nNetIter<m_pRplData->vNet.size();++nNetIter){
            for(unsigned nNetNodeIter=0;nNetNodeIter<m_pRplData->vNet[nNetIter]->vNetNode.size();++nNetNodeIter){
                delete m_pRplData->vNet[nNetIter]->vNetNode[nNetNodeIter];
            }
            delete m_pRplData->vNet[nNetIter];
        }
        for(unsigned nPlNodeIter=0;nPlNodeIter<m_pRplData->vPlNode.size();++nPlNodeIter) delete m_pRplData->vPlNode[nPlNodeIter];
        for(unsigned nRowIter=0;nRowIter<m_pRplData->vRow.size();++nRowIter) delete m_pRplData->vRow[nRowIter];
        delete m_pRplData;
        m_pRplData = 0;
    }
}
//----------------------------------------------------------------------------------------------------------------------//
void rplParser_C::parse()
{
    parseAux();
    parseNode();
    //parseNet();
    parsePl();
    parseScl();
}
//----------------------------------------------------------------------------------------------------------------------//
void rplParser_C::getLine(ifstream& p_fStream, char* p_caBuffer, int p_nBufferSize)
{
    m_fInputAux.getline(p_caBuffer,p_nBufferSize);
    while(strlen(p_caBuffer)==0)
    {
        m_fInputAux.getline(p_caBuffer,p_nBufferSize);
    }
}
//----------------------------------------------------------------------------------------------------------------------//
void rplParser_C::parseAux()
{
    clock_t start, end;
    start = clock();
    char input[256];
    char *pCh;
    string strTmp, strPath="", strNewPath="";
    string strNode,strNet, strWts, strPl, strScl, strShape, strRoute;
    string strNewNode,strNewNet, strNewWts, strNewPl, strNewScl, strNewShape, strNewRoute;
    bool bDot=false;
    m_fInputAux.open(m_caInputAux, ios::in);
    if(!m_fInputAux)
    {
        WarnMsg("[Abort-Parser #1] - Input File: %s Can Not Be Opened\n",m_caInputAux);
        assert(0);
    }
    //getLine(m_fInputAux,buffer,256);
    m_fInputAux>>strTmp>>strTmp>>strNode>>strNet>>strWts>>strPl>>strScl>>strShape>>strRoute;

    //sscanf(buffer,"RowBasedPlacement : %s %s %s %s %s %s %s\n",m_caInputNode,m_caInputNet,
    //        m_caInputWts, m_caInputPl, m_caInputScl,
    //        m_caInputShape, m_caInputRoute);

    strcpy(input,m_caInputAux);
    pCh = strtok(input,"/");
    while(pCh!=NULL)
    {
        strTmp = pCh; 
        for(unsigned int nIter=0;nIter<strTmp.size()-1;++nIter) 
        {
            if((strTmp[nIter]=='.')&&(strTmp[nIter+1]!='.'))
            {
                bDot=true;
                break;
            }
        }
        if(bDot) break;
        strPath = strPath + pCh + "/";
        pCh = strtok(NULL,"/");
    }

    strNewNode = strPath + strNode;
    m_caInputNode = strNewNode;

    strNewNet = strPath + strNet;
    m_caInputNet = strNewNet;
    
    strNewPl = strPath + strPl;
    m_caInputPl = strNewPl;

    strNewScl = strPath + strScl;
    m_caInputScl = strNewScl;
    
    end = clock();
    
    InfoMsg("[Parser - 1-1] - Parsing Following Files:- (%lf sec.)\n",(double)(end-start)/CLOCKS_PER_SEC);
    InfoMsg("[Parser - 1-2] - Node  File: %s\n",m_caInputNode.c_str());
    //InfoMsg("[Parser - 1-3] - Net   File: %s\n",m_caInputNet.c_str());
    InfoMsg("[Parser - 1-4] - Pl    File: %s\n",m_caInputPl.c_str());
    InfoMsg("[Parser - 1-5] - Scl   File: %s\n",m_caInputScl.c_str());
    m_fInputAux.close();
}
//----------------------------------------------------------------------------------------------------------------------//
void rplParser_C::parseNode()
{
    clock_t start, end;
    start = clock();
    rplNode_S* pNode;
    char buffer[256]={0};
    char temp[32]={0};
    int nNumNode=0,nNumTerm=0,nCount=0;
    int nNode=0,nFix=0,nOverlapFix=0;
    m_fInputNode.open(m_caInputNode.c_str(),ios::in);
    if(!m_fInputNode)
    {
        WarnMsg("[Abort-Parser #2] - Input File: %s Can Not Be Opened\n",m_caInputNode.c_str());
        assert(0);
    }
    m_fInputNode.getline(buffer,256);
    m_fInputNode.getline(buffer,256);
    m_fInputNode.getline(buffer,256);
    m_fInputNode.getline(buffer,256);
    m_fInputNode.getline(buffer,256);
    sscanf(buffer,"NumNodes : %d\n",&nNumNode);
    m_fInputNode.getline(buffer,256);
    sscanf(buffer,"NumTerminals : %d\n",&nNumTerm);
    m_fInputNode.getline(buffer,256);
    for(int nNodeIter=0;nNodeIter<nNumNode;++nNodeIter)
    {
        pNode = new rplNode_S;
        m_fInputNode.getline(buffer,256);
        sscanf(buffer," %s %d %d %s\n",pNode->nodeName,&pNode->width,&pNode->height, temp);
        if(strstr(temp,"terminal_NI")!=NULL)
        {
            nOverlapFix ++;
            pNode->nodeType = 2;
        }
        else if(strstr(temp,"terminal")!=NULL)
        {
            nFix++;
            pNode->nodeType = 1;
        }
        else
        {
            nNode++;
            pNode->nodeType = 0;
        }
        m_pRplData->vNode.push_back(pNode);
        //InfoMsg("[Parser - 2-1] - Parsing Node - %d - Parsed\r",nCount);
        nCount++;
    }
    m_pRplData->nMoveNode = nNode;
    m_pRplData->nFixNode = nFix;
    m_pRplData->nOverlapFixNode = nOverlapFix;
    m_fInputNode.close();
    end = clock();
    InfoMsg("[Parser - 2] - Parsing Node - %d Node, %d Fixed Node, %d Overlap Fixed Node- Parsed - (%lf sec.)\n",nNode,nFix,nOverlapFix,
            (double)(end-start)/CLOCKS_PER_SEC);
}
//----------------------------------------------------------------------------------------------------------------------//
void rplParser_C::parseNet()
{
    clock_t start, end;
    start = clock();
    char buffer[256];
    char caNetName[32];
    char caNetNodeName[32];
    char cNetNodeIO;
    int nNumNet, nNumPin, nNumNetNode,nCount=0;
    rplNetNode_S* pNetNode(0);
    rplNet_S* pNet(0);
    m_fInputNet.open(m_caInputNet.c_str(),ios::in);
    if(!m_fInputNet)
    {
        WarnMsg("[Abort-Parser #3] - Input File: %s Can Not Be Opened\n",m_caInputNet.c_str());
        assert(0);
    }
    m_fInputNet.getline(buffer,256);
    m_fInputNet.getline(buffer,256);
    m_fInputNet.getline(buffer,256);
    m_fInputNet.getline(buffer,256);
    m_fInputNet.getline(buffer,256);
    sscanf(buffer,"NumNets : %d\n",&nNumNet);
    m_fInputNet.getline(buffer,256);
    sscanf(buffer,"NumPins : %d\n",&nNumPin);
    m_fInputNet.getline(buffer,256);
    for(int nNetIter=0;nNetIter<nNumNet;++nNetIter)
    {
        m_fInputNet.getline(buffer,256);
        pNet = new rplNet_S;
        sscanf(buffer,"NetDegree : %d %s\n",&nNumNetNode,caNetName);
        if(nNumNetNode==0)
        {
            WarnMsg("0 Node Net\n");
        }
        pNet->netName = caNetName;
        for(int nNetNodeIter=0;nNetNodeIter<nNumNetNode;++nNetNodeIter)
        {
            pNetNode = new rplNetNode_S;
            m_fInputNet.getline(buffer,256);
            sscanf(buffer," %s %c : %f %f\n",caNetNodeName,&cNetNodeIO,&pNetNode->netNodeX,&pNetNode->netNodeY);
            pNetNode->netNodeName = caNetNodeName;
            if(cNetNodeIO=='O') pNetNode->netNodeIO = 0;
            else if(cNetNodeIO=='I') pNetNode->netNodeIO = 1;
            else if(cNetNodeIO=='B') pNetNode->netNodeIO = 2;
            else
            {
                WarnMsg("[Abort-Parser #4] - NetNode '%c' is not 'I' or 'O' or 'B'\n",cNetNodeIO);
                assert(0);
            }
            pNet->vNetNode.push_back(pNetNode);
        }
        nCount++;
        m_pRplData->vNet.push_back(pNet);

    }
    end = clock();
    InfoMsg("[Parser - 3] - Parsing Net - %d - Parsed - (%lf sec.)\n",nCount,(double)(end-start)/CLOCKS_PER_SEC);
    m_fInputNet.close();
}
//----------------------------------------------------------------------------------------------------------------------//
void rplParser_C::parsePl()
{
    clock_t start, end;
    start = clock();
    char buffer[256];
    char caNodeName[32];
    char caTemp[32];
    char *pCh;
    int nCount=0;
    rplPlNode_S* pPlNode(0);
    m_fInputPl.open(m_caInputPl.c_str(),ios::in);

    if(!m_fInputNet)
    {
        WarnMsg("[Abort-Parser #5] - Input File: %s Can Not Be Opened\n",m_caInputPl.c_str());
        assert(0);
    }
    m_fInputPl.getline(buffer,256);
    m_fInputPl.getline(buffer,256);
    m_fInputPl.getline(buffer,256);
    m_fInputPl.getline(buffer,256);
    while(m_fInputPl.peek()!=EOF)
    {
        pPlNode = new rplPlNode_S;
        m_fInputPl.getline(buffer,256);
        sscanf(buffer,"%s %d %d : %s\n",caNodeName,&pPlNode->plNodeX,&pPlNode->plNodeY,caTemp);
        if((pCh=strstr(buffer,"/FIXED"))!=NULL) pPlNode->plNodeFixed=1;
        else pPlNode->plNodeFixed=0;
        pPlNode->plNodeName = caNodeName;
        m_pRplData->vPlNode.push_back(pPlNode);
        nCount++;
    } 
    end = clock();
    InfoMsg("[Parser - 4] - Parsing Pl - %d - Parsed - (%lf sec.)\n",nCount,(double)(end-start)/CLOCKS_PER_SEC);
    m_fInputPl.close();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------//
void rplParser_C::parseScl()
{
    clock_t start, end;
    start = clock();
    char buffer[256];
    int nNumRow, nCount=0;
    int nTotalWidth=0, nMinWidth = INT_MAX;
    int nMaxHeight=INT_MIN;
    int nMinHeight=INT_MAX;

    rplRow_S* pRow(0);
    m_fInputScl.open(m_caInputScl.c_str(),ios::in);
    if(!m_fInputNet)
    {
        WarnMsg("[Abort-Parser #6] - Input File: %s Can Not Be Opened\n",m_caInputScl.c_str());
        assert(0);
    }
    m_fInputScl.getline(buffer,256);
    m_fInputScl.getline(buffer,256);
    m_fInputScl.getline(buffer,256);
    m_fInputScl.getline(buffer,256);
    m_fInputScl.getline(buffer,256);
    sscanf(buffer,"NumRows : %d\n", &nNumRow);
    m_fInputScl.getline(buffer,256);

    for(int nRowIter=0;nRowIter<nNumRow;++nRowIter) 
    {
        pRow = new rplRow_S;
        m_fInputScl.getline(buffer,256); //CoreRow Horizontal
        m_fInputScl.getline(buffer,256);
        sscanf(buffer," Coordinate : %d\n", &pRow->rowCoord); //Y-Coordinate
        m_fInputScl.getline(buffer,256);
        sscanf(buffer," Height : %d\n", &pRow->rowHeight);
        m_fInputScl.getline(buffer,256);
        sscanf(buffer," Sitewidth : %d\n", &pRow->rowSiteWidth);
        m_fInputScl.getline(buffer,256);
        sscanf(buffer," Sitespacing : %d\n", &pRow->rowSiteSpace);
        m_fInputScl.getline(buffer,256);
        sscanf(buffer," Siteorient : %c\n", &pRow->rowSiteOrient);
        m_fInputScl.getline(buffer,256);
        sscanf(buffer," Sitesymmetry : %c\n", &pRow->rowSiteSym);
        m_fInputScl.getline(buffer,256);
        sscanf(buffer," SubrowOrigin : %d NumSites : %d\n", &pRow->rowSubRowOrig,&pRow->numSite);
        m_pRplData->vRow.push_back(pRow);
        m_fInputScl.getline(buffer,256); //End
        nTotalWidth = nTotalWidth + (pRow->rowSiteWidth * pRow->numSite);
        nMinWidth = min(nMinWidth, (pRow->rowSiteWidth * pRow->numSite));
        if(pRow->rowCoord > nMaxHeight) nMaxHeight = pRow->rowCoord;
        if(pRow->rowCoord < nMinHeight) nMinHeight = pRow->rowCoord;
        nCount++;
    }
    m_pRplData->nChipWidth = (int)(nTotalWidth)/nNumRow;
    m_pRplData->nChipHeight = nMaxHeight - nMinHeight;
    end = clock();
    InfoMsg("[Parser - 5] - Parsing Row - %d - Parsed, Chip Width = %d, Chip Height = %d - (%lf sec.)\n",nCount, 
            int(nTotalWidth/nNumRow),
            nMaxHeight-nMinHeight,
            (double)(end-start)/CLOCKS_PER_SEC);
    m_fInputScl.close();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------//
void rplParser_C::dump(Dumper& p_rDumper)
{
    rplNode_S* pNode(0);
    rplPlNode_S* pPlNode(0);
    rplRow_S* pRow(0);

    p_rDumper.write("//NODE\n");
    for(unsigned int nNodeIter=0;nNodeIter<m_pRplData->vNode.size();++nNodeIter)
    {
        pNode = m_pRplData->vNode[nNodeIter];
        p_rDumper.write("\t%s X:%d Y:%d Type:%d\n",pNode->nodeName,pNode->width,pNode->height,pNode->nodeType);
    }
    p_rDumper.write("//Placement Node\n");
    for(unsigned int nPlNodeIter=0;nPlNodeIter<m_pRplData->vPlNode.size();++nPlNodeIter)
    {
        pPlNode = m_pRplData->vPlNode[nPlNodeIter];
        p_rDumper.write("\t%s X:%d Y:%d Fixed:%d\n",pPlNode->plNodeName.c_str(),pPlNode->plNodeX,pPlNode->plNodeY,pPlNode->plNodeFixed);
    } 
    p_rDumper.write("//ROW\n");
    for(unsigned int nRowIter=0;nRowIter<m_pRplData->vRow.size();++nRowIter)
    {
        pRow = m_pRplData->vRow[nRowIter];
        p_rDumper.write("\tCoord:%d H:%d SiteW:%d SiteSpc:%d SiteOrnt:%c SiteSym:%c SubRowOrig:%d NumSite:%d\n",
                pRow->rowCoord,pRow->rowHeight,pRow->rowSiteWidth,pRow->rowSiteSpace,pRow->rowSiteOrient,
                pRow->rowSiteSym, pRow->rowSubRowOrig, pRow->numSite);
    } 
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------//

