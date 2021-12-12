#include <iostream>
#include "message.h"
#include "rplParser.h"

int main(int argc, char* argv[])
{
    /*Preliminary Information*/
    /*std::cout << "     ################################################################" << std::endl;
    std::cout << "     #                                                              #" << std::endl; 
    std::cout << "     #                        [5037 PDA - Lab4]                     #" << std::endl; 
    std::cout << "     #          Placement Legalization with Minimal Displacement    #" << std::endl; 
    std::cout << "     #   Compiled On : "<<__DATE__<<" at "<<__TIME__<<" on "<<sizeof(char*)*8<<"-bit machine    #" << std::endl;
    std::cout << "     #                    VLSI Design Automation Lab                #" << std::endl; 
    std::cout << "     #              Institute of Electronics Engineering            #" << std::endl; 
    std::cout << "     #                  National Chiao Tung University              #" << std::endl; 
    std::cout << "     #                                                              #" << std::endl; 
    std::cout << "     ################################################################" << std::endl;
    std::cout << std::endl;*/

    clock_t start, end;
    start = clock();
    rplParser_C* pRplParser = new rplParser_C(argv[1]);
    pRplParser->parse();
    Dumper dumpParser("dumpParser.txt"); //Optional
    pRplParser->dump(dumpParser);        //Optional
    
    /* Code Begin */

    //Example Code
    rplData_S *pData = pRplParser->getRplData();
    rplNode_S* pNode(0);
    printf("node - width height\n");
    for(unsigned int nNodeIter=0;nNodeIter<10;++nNodeIter)//pData->vNode.size();++nNodeIter)
    {
        pNode = pData->vNode[nNodeIter];
        printf("%s - %d %d\n",pNode->nodeName, pNode->width, pNode->height);
    }
     
    delete pRplParser;
    /* Code End */

    end = clock();
    InfoMsg("Execution Time: %lf seconds\n", (double)(end-start)/CLOCKS_PER_SEC);
}
