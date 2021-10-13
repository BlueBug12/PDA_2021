#include "corner_stitching.hpp"

CornerStitching::CornerStitching(int width, int height):_width(width), _height(height){
    _start_tile = new Tile(0,0,_width,_height,-1);
    _tile_num = 1;
}

Tile* CornerStitching::findPoint(int x, int y){
    //be careful of corner case(point at edge))
    if(x>_width || x<0 || y>_height || y<0){
        std::cerr << "Error: findPoint out of range." << std::endl;
        exit(1);
    }
    Tile* current = _start_tile;
    while((inTileH(*current, x)!=0) && (inTileV(*current,y)!=0)){
       current = searchH(current, x);
       current = searchV(current, y);
    }

    //deal with the corner cases:
    //case1: the point is at the right edge of tile (not a corner point)
    if(x == current->rightX()){
        current = current->tr;
        while(current->y >= y)
            current = current->lb;
    }else if(y == current->y){//case2: the point is at the bottom edge of tile (not a corner point)
        current = current->lb;
        while(current->rightX() <= x)
            current = current->tr;
    }
    //case3.1: the point is at bottom-right corner
    //case3.2: the point is at bottom-left corner
    //case3.3: the point is at top-right corner
    //all case3s are included in the previoud code.
    return current;
}

Tile* CornerStitching::searchH(Tile* t_ptr, const int x){
    if(!t_ptr){
        std::cerr<<"Error: NULL pointer in searchH."<<std::endl;
        exit(1);
    }

#ifdef DEBUG
    //std::cout<<"  searchH:"<<t_ptr->x<<" "<<t_ptr->y<<std::endl;
#endif
    int state = inTileH(*t_ptr,x);
    if(state==0)//x is in the range of tile t
        return t_ptr;
    if(state==-1)//the position of tile t is at right hand side of x
        return searchH(t_ptr->bl,x);
    else//the position of tile t is at left hand side of x
        return searchH(t_ptr->tr,x);
}

Tile* CornerStitching::searchV(Tile* t_ptr, const int y){
    if(!t_ptr){
        std::cerr<<"Error: NULL pointer in searchV."<<std::endl;
        exit(1);
    }
#ifdef DEBUG
    //std::cout<<"  searchV:"<<t_ptr->x<<" "<<t_ptr->y<<std::endl;
#endif
    int state = inTileV(*t_ptr,y);
    if(state==0)//y is in the range of tile t
        return t_ptr;
    else if(state==-1)//the position of tile t is higher than y
        return searchV(t_ptr->lb,y);
    else// the positio of tile t is lower than y
        return searchH(t_ptr->rt,y);
}

inline void counter(const Tile* t, int& space_num, int& block_num){
    if(t->index==-1)
        ++space_num;
    else
        ++block_num;
}

void CornerStitching::countNeighbor(Tile* t, int& space_num, int& block_num){
    space_num = 0;
    block_num = 0;
    Tile* temp;

    //top
    temp = t->rt;
    while(temp && temp->rightX() > t->x){
        counter(temp,space_num,block_num);
        temp = temp->bl;
    }
    //left
    temp = t->bl;
    while(temp && temp->y < t->topY()){
        counter(temp,space_num,block_num);
        temp = temp->rt;    
    }
    //bottom
    temp = t->lb;
    while(temp && temp->x < t-> rightX()){ 
        counter(temp,space_num,block_num);
        temp = temp->tr;
    }
    //right
    temp = t->tr;
    while(temp && temp->topY() > t->y){
        counter(temp,space_num,block_num);
        temp = temp->lb;
    }
}

bool CornerStitching::searchArea(int left_x, int bottom_y, int width, int height){
    //may not need
    return true;
}

bool CornerStitching::insertTile(Tile* t){
    std::cout<<"insert tile "<<t->x<<" "<<t->y<<" "<<t->width<<" "<<t->height<<" "<<t->index<<std::endl;
    if(!searchArea(t->x,t->y,t->width,t->height))
        return false;
    //search the tile that contains top edge
    Tile* top = findPoint((t->x + t->width/2),t->topY());//set the top-center of tile as target position
#ifdef DEBUG
    std::cout<<t->x+t->width/2<<","<<t->topY()<<"  findPoint top tile:"<<top->x<<" "<<top->y<<std::endl;
#endif
    topSplit(top,t);

    //search the tile that contains bottom edge
    Tile* bottom = findPoint((t->x + t->width/2),t->y+1);//+1 to avoid corner case
#ifdef DEBUG
    std::cout<<t->x+t->width/2<<","<<t->y+1<<"  findPoint bottom tile:"<<bottom->x<<" "<<bottom->y<<std::endl;
#endif
    bottomSplit(bottom,t);
    
    insertTile(top,bottom,t,NULL,NULL);
    tiles.push_back(t); 
    
    #ifdef DEBUG
    for(size_t i=0;i<tiles.size();++i){
        t = tiles.at(i); 
        int space_num, block_num;
        countNeighbor(t,space_num,block_num);
        std::cout<<"    Tile "<<t->index<<std::endl;
        std::cout<<"    Counting result: "<<block_num<<" "<<space_num<<std::endl;
        if(t->tr)
            std::cout<<"    tr: "<<t->tr->x<<" "<<t->tr->y<<" "<<t->tr->width<<" "<<t->tr->height<<" "<<t->tr->index<<std::endl;
        else
            std::cout<<"    tr: "<<"NULL"<<std::endl;
        if(t->rt)
            std::cout<<"    rt: "<<t->rt->x<<" "<<t->rt->y<<" "<<t->rt->width<<" "<<t->rt->height<<" "<<t->rt->index<<std::endl;
        else
            std::cout<<"    rt: "<<"NULL"<<std::endl;
        if(t->bl)
            std::cout<<"    bl: "<<t->bl->x<<" "<<t->bl->y<<" "<<t->bl->width<<" "<<t->bl->height<<" "<<t->bl->index<<std::endl;
        else
            std::cout<<"    bl: "<<"NULL"<<std::endl;
        if(t->lb)
            std::cout<<"    lb: "<<t->lb->x<<" "<<t->lb->y<<" "<<t->lb->width<<" "<<t->lb->height<<" "<<t->lb->index<<std::endl;
        else
            std::cout<<"    lb: "<<"NULL"<<std::endl;
    }
    #endif

    return true;

}

bool CornerStitching::insertTile(const int x, const int y, const int width, const int height, const int i){
    Tile* t = new Tile(x,y,width,height,i);
    return insertTile(t);
}

std::vector<Tile> CornerStitching::collectAllTiles(){}


void CornerStitching::topSplit(Tile* space, Tile* block){
    /*
    if(y == space->y || y == space->y+t->height){
        std::cerr<<"Error: no need to splitH."<<std::endl;
        exit(1);
    }*/
    
    if(space->index!=-1){
        std::cerr<<"Error: the tile is not a space."<<std::endl;
        exit(1);
    }
    if(block->topY()==space->topY()){//no need to split
        Tile* temp = space->rt;
        while(temp && temp->x >= block->rightX())//find the right-top tile for block
            temp = temp->bl;
        block->rt = temp;
        while(temp && temp->x >= block->x){//change the pointers that point to the original tile(if exists)
            temp->lb = block;
            temp = temp->bl;
        }
        return;
    }

    //set t2 as the space that doesn't contain block
    Tile* t1 = space;
    Tile* t2 = new Tile(t1); 

    t2->y = block->topY();
    t2->height = t1->topY() - block->topY();
    t2->lb = t1;
    while(t2->bl && t2->bl->topY() <= t2->y)
       t2->bl = t2->bl->rt;
    //set left edge to t2
    Tile* temp = t2->bl;
    while(temp && temp->topY()<=t2->topY()){
        temp->tr = t2;
        temp = temp->rt;
    }
    //set top edge to t2
    temp = t2->rt;
    while(temp && temp->x >= t2->x){
        temp->lb = t2;
        temp = temp->bl;
    }
    //set right edge to t2
    temp = t2->tr;
    while(temp && temp->y >= t2->y){
        temp->bl = t2;
        temp = temp->lb;
    }

    t1->height = block->topY() - t1->y;
    t1->rt = t2;
    while(t1->tr && t1->tr->y >= t1->topY())
       t1->tr = t1->tr->lb;

    block->rt = t2;
}

void CornerStitching::bottomSplit(Tile* space, Tile* block){
    /*if(y == space->y || y == space->y+t->height){
        std::cerr<<"Error: no need to splitH."<<std::endl;
        exit(1);
    }*/
    
    if(space->index!=-1){
        std::cerr<<"Error: the tile is not a space."<<std::endl;
        exit(1);
    }
    if(block->y == space->y){//no need to split
        Tile* temp = space->lb;
        while(temp && temp->rightX() <= block->x)
            temp = temp->tr;
        block->lb = temp;
        while(temp && temp->rightX() <= block->rightX()){
            temp->rt = block;
            temp = temp->tr;
        }
        return;
    }

    //set t2 as the space that doesn't contain block
    Tile* t1 = space;
    Tile* t2 = new Tile(t1); 

    t2->height = block->y - t2->y;
    t2->rt = t1;
    while(t2->tr && t2->tr->y >= t2->topY())
       t2->tr = t2->tr->lb;
    
    //set left edge to t2
    Tile* temp = t2->bl;
    while(temp && temp->topY()<=t2->topY()){
        temp->tr = t2;
        temp = temp->rt;
    }
    //set bottom edge to t2
    temp = t2->lb;
    while(temp && temp->rightX()<=t2->rightX()){
        temp->rt = t2;
        temp = temp->tr;
    }
    //set right edge to t2
    temp = t2->tr;
    while(temp && temp->y >= t2->y){
        temp->bl = t2;
        temp = temp->lb;
    }


        
    t1->height = t1->topY() - block->y;
    t1->y = block->y;
    t1->lb = t2;
    while(t1->bl && t1->lb->topY() <= t1->y)
       t1->bl = t1->bl->rt;

    block->lb = t2;

#ifdef DEBUG
    std::cout<<"bottom split t1:"<<t1->x<<" "<<t1->y<<std::endl;
    std::cout<<"bottom split t2:"<<t2->x<<" "<<t2->y<<std::endl;
#endif
}

inline bool CornerStitching::crossOverlapping(Tile* space, Tile* block){
    if(!space)
        return true;
    if(space->x<=block->x && space->rightX()>=block->rightX() && space->y>=block->y && space->topY()<=block->topY()){
        if(space->index!=-1 || block->index==-1){
            std::cerr << "Error: wrong tile type.(crossOverlapping))"<<std::endl;
            exit(1);
        }else{
            return true;
        }
    }else
        return false;
}

void CornerStitching::insertTile(Tile* top, Tile* bottom, Tile* block, Tile* pre_l, Tile* pre_r){
    if(!top)
        return;
    if(top->index!=-1 || bottom->index!=-1 || block->index==-1){
        std::cerr<<"Error: wrong tile type.(spaceMerge))"<<std::endl;
        exit(1);
    }

    Tile* temp;
    Tile* next;
    Tile* l_top = NULL;
    Tile* r_top = NULL;

    int r_width = top->rightX() - block->rightX();
    int l_width = block->x - top->x;
    bool first = false;
    if(top->topY() == block->topY()){//first recursion
        first = true;    
    }
    if(top == bottom){//last recursion
        next = NULL;//set the termination condition  
    }else{//find next tile occupied by the block
        next = top->lb;
        while(!crossOverlapping(next,block))
            next = next->tr;
    }
    //both edges of the space are aligned with block
    if(r_width == 0 && l_width == 0){
        if(first)
            block->tr = top->tr;
        if(!next)
            block->bl = top->bl;

        temp = top->tr;
        while(temp && temp->bl == top){
            temp->bl = block;
            temp = temp->lb;
        }
    
        temp = top->bl;
        while(temp && temp->tr == top){
            temp->tr = block;
            temp = temp->rt;
        }
        
        delete top;

    }else if(r_width == 0){//the right edge of space is aligned with the right edge of block
        if(first)
            block->tr = top->tr;
        if(!next)
            block->bl = top;

        //set the relative pointers point to block instead of the original space(top)
        temp = top->tr;
        while(temp && temp->bl == top){
            temp->bl = block;
            temp = temp->lb;
        }
        top->width -= block->width;
        top->tr = block;
        while(top->rt && top->rt->x >= top->rightX())
            top->rt = top->rt->bl;
        l_top = top;
    }else if(l_width == 0){//the left edge of space is aligned with the right edge of block
        if(first)
            block->tr = top;
        if(!next)
            block->bl = top->bl;

        temp = top->bl;
        while(temp && temp->tr == top){
            temp->tr = block;
            temp = temp->rt;
        }
        top->width -= block->width;
        top->bl = block;
        while(top->lb && top->lb->rightX() <= top->x)
            top->lb = top->lb->tr;
        r_top = top;
    }else{
        r_top = new Tile(top);
        l_top = top;
        if(first)
            block->tr = r_top;
        if(!next)
            block->bl = top;

        r_top->x = block->rightX();
        r_top->width = top->rightX()-block->rightX();
        r_top->bl = block;
        while(r_top->lb && r_top->lb->rightX() <= r_top->x)
            r_top->lb = r_top->lb->tr;
        
        l_top->width = block->x - l_top->x;
        l_top->tr = block;
        while(l_top->rt && l_top->rt->x >= l_top->rightX())
            l_top->rt = l_top->rt->bl;
    }
    mergeSpace(pre_l, l_top);
    mergeSpace(pre_r, r_top);
    insertTile(next,bottom,block,l_top,r_top);
}

bool CornerStitching::mergeSpace(Tile* pre, Tile* cur){
    //preserve the current one if merge
    if(!pre || !cur)
        return false;
    if(pre->index!=-1 || cur->index!=-1){
        std::cerr<<"Error: unexpected block to merge."<<std::endl;
        exit(1);
    }
    if(pre->y != cur->topY()){
        std::cerr<<"Error: wrong sequence."<<std::endl;
        exit(1);
    }
    if(pre->width==cur->width && pre->x == cur->x){
        Tile* temp;
        //update all the pointers that point to pre
        //left edge
        temp = pre->bl;
        while(temp && temp->tr == pre){
            temp->tr = cur;
            temp = temp->rt;
        }
        //right edge
        temp = pre->tr;
        while(temp && temp->bl == pre){
            temp->bl = cur;
            temp = temp->lb;
        }
        //top edge
        temp = pre->rt;
        while(temp && temp->lb == pre){
            temp->lb = cur;
            temp = temp->bl;
        }
        cur->rt = pre->rt;
        cur->tr = pre->tr;
        delete pre;
        return true;
    }else{
        return false;
    }

}
