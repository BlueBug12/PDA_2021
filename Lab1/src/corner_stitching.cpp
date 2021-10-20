#include "corner_stitching.hpp"

inline int inTileH(Tile & t, const int x){
    if(x >= t.x && x < t.rightX())//x is in the range of tile t
        return 0;
    else if(x < t.x)//the position of tile t is at right hand side of x
        return -1;
    else//the position of tile t is at left hand side of x
        return 1;
}

inline int inTileV(Tile & t, const int y){
    if(y > t.y && y <= t.topY())//y is in the range of the tile t
        return 0;
    else if(y <= t.y)//the position of tile t is higher than y
        return -1;
    else//the position of tile t is lower than y
        return 1;
}

void check(Tile *t){
    Tile *temp = t->bl;
    if(temp && (temp->rightX()!=t->x || temp->topY()<=t->y || temp->y > t->y)){
        std::cerr<<"Error: found wrong pointer when check left."<<std::endl;
        std::cerr<<t->x<<" "<<t->y<<" "<<t->width<<" "<<t->height<<" "<<t->index<<std::endl;
        std::cerr<<temp->x<<" "<<temp->y<<" "<<temp->rightX()<<" "<<temp->topY()<<" "<<temp->index<<std::endl;
        exit(1);
    }

    temp = t->tr;
    if(temp && (temp->x!=t->rightX() || temp->topY()<t->topY() || temp->y >= t->topY())){
        std::cerr<<"Error: found wrong pointer when check right."<<std::endl;
        std::cerr<<t->x<<" "<<t->y<<" "<<t->width<<" "<<t->height<<" "<<t->index<<std::endl;
        std::cerr<<temp->x<<" "<<temp->y<<" "<<temp->rightX()<<" "<<temp->topY()<<" "<<temp->index<<std::endl;
        exit(1);
    }

    temp = t->rt;
    if(temp && (temp->y!=t->topY() || temp->x >= t->rightX() || temp->rightX() < t->rightX())){
        std::cerr<<"Error: found wrong pointer when check top."<<std::endl;
        std::cerr<<t->x<<" "<<t->y<<" "<<t->rightX()<<" "<<t->topY()<<" "<<t->index<<std::endl;
        std::cerr<<temp->x<<" "<<temp->y<<" "<<temp->rightX()<<" "<<temp->topY()<<" "<<temp->index<<std::endl;
        exit(1);
    }

    temp = t->lb;
    if(temp && (temp->topY()!=t->y || temp->rightX() <= t->x || temp->x > t->x)){
        std::cerr<<"Error: found wrong pointer when check bottom."<<std::endl;
        std::cerr<<t->x<<" "<<t->y<<" "<<t->width<<" "<<t->height<<" "<<t->index<<std::endl;
        std::cerr<<temp->x<<" "<<temp->y<<" "<<temp->rightX()<<" "<<temp->topY()<<" "<<temp->index<<std::endl;
        exit(1);
    }

}

void debugger(Tile* t){
    
        if(t->tr)
            std::cout<<"        tr: "<<t->tr->x<<" "<<t->tr->y<<" "<<t->tr->width<<" "<<t->tr->height<<" "<<t->tr->index<<std::endl;
        else
            std::cout<<"        tr: "<<"NULL"<<std::endl;
        if(t->rt)
            std::cout<<"        rt: "<<t->rt->x<<" "<<t->rt->y<<" "<<t->rt->width<<" "<<t->rt->height<<" "<<t->rt->index<<std::endl;
        else
            std::cout<<"        rt: "<<"NULL"<<std::endl;
        if(t->bl)
            std::cout<<"        bl: "<<t->bl->x<<" "<<t->bl->y<<" "<<t->bl->width<<" "<<t->bl->height<<" "<<t->bl->index<<std::endl;
        else
            std::cout<<"        bl: "<<"NULL"<<std::endl;
        if(t->lb)
            std::cout<<"        lb: "<<t->lb->x<<" "<<t->lb->y<<" "<<t->lb->width<<" "<<t->lb->height<<" "<<t->lb->index<<std::endl;
        else
            std::cout<<"        lb: "<<"NULL"<<std::endl;
}
CornerStitching::CornerStitching(int width, int height):_width(width), _height(height){
    _start_tile = new Tile(0,0,_width,_height,-1);
    all.insert(_start_tile);
    _tile_num = 1;
}


Tile* CornerStitching::findPoint(int x, int y, Tile *hint){
    //be careful of corner case(point at edge))
    if(x>_width || x<0 || y>_height || y<0){
        std::cout << "Warning: findPoint out of range." << std::endl;
        return NULL;
    }
    if(!hint)
        hint = _start_tile;
    while((inTileH(*hint, x)!=0) || (inTileV(*hint,y)!=0)){
       hint = searchH(hint, x);
       hint = searchV(hint, y);
    }
    return hint;
}

Tile* CornerStitching::searchH(Tile* t_ptr, const int x){
    if(!t_ptr){
        std::cerr<<"Error: NULL pointer in searchH."<<std::endl;
        exit(1);
    }

    int state = inTileH(*t_ptr,x);
    if(state == 0)//x is in the range of tile t
        return t_ptr;
    else if(state == -1)//the position of tile t is at right hand side of x
        return searchH(t_ptr->bl,x);
    else//the position of tile t is at left hand side of x
        return searchH(t_ptr->tr,x);
}

Tile* CornerStitching::searchV(Tile* t_ptr, const int y){
    if(!t_ptr){
        std::cerr<<"Error: NULL pointer in searchV."<<std::endl;
        exit(1);
    }
    int state = inTileV(*t_ptr,y);
    if(state == 0)//y is in the range of tile t
        return t_ptr;
    else if(state == -1)//the position of tile t is higher than y
        return searchV(t_ptr->lb,y);
    else// the positio of tile t is lower than y
        return searchV(t_ptr->rt,y);
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
    std::cout<<"insert tile "<<t->index<<std::endl;
    if(!searchArea(t->x,t->y,t->width,t->height))
        return false;
    //search the tile that contains top edge
    Tile* top = findPoint((t->x + t->width/2),t->topY());//set the top-center of tile as target position
    topSplit(top,t);

    //search the tile that contains bottom edge
    Tile* bottom = findPoint((t->x + t->width/2),t->y+1);//+1 to avoid corner case
    bottomSplit(bottom,t);

    Tile *pre_l = findPoint(t->x-1,t->topY()+1,top);
    Tile *pre_r = findPoint(t->rightX(),t->topY()+1,top);

    insertTile(top,bottom,t,pre_l,pre_r);
    tiles.push_back(t); 
    _start_tile = tiles.front();

#ifdef DEBUG
    for(size_t i=0;i<tiles.size();++i){
        t = tiles.at(i); 
        int space_num = 0;
        int block_num = 0;
        //countNeighbor(t,space_num,block_num);
        std::cout<<"    Tile "<<t->index<<std::endl;
        std::cout<<"    Counting result: "<<block_num<<" "<<space_num<<std::endl;
        if(t->tr){
            std::cout<<"    tr: "<<t->tr->x<<" "<<t->tr->y<<" "<<t->tr->width<<" "<<t->tr->height<<" "<<t->tr->index<<std::endl;
            //debugger(t->tr);
        }
        else
            std::cout<<"    tr: "<<"NULL"<<std::endl;
        if(t->rt){
            std::cout<<"    rt: "<<t->rt->x<<" "<<t->rt->y<<" "<<t->rt->width<<" "<<t->rt->height<<" "<<t->rt->index<<std::endl;
            //debugger(t->rt);
        }
        else
            std::cout<<"    rt: "<<"NULL"<<std::endl;
        if(t->bl){
            std::cout<<"    bl: "<<t->bl->x<<" "<<t->bl->y<<" "<<t->bl->width<<" "<<t->bl->height<<" "<<t->bl->index<<std::endl;
            //debugger(t->bl);
        }
        else
            std::cout<<"    bl: "<<"NULL"<<std::endl;
        if(t->lb){
            std::cout<<"    lb: "<<t->lb->x<<" "<<t->lb->y<<" "<<t->lb->width<<" "<<t->lb->height<<" "<<t->lb->index<<std::endl;
            //debugger(t->lb);
        }
        else
            std::cout<<"    lb: "<<"NULL"<<std::endl;
    }
    for(Tile* it: all)
        check(it);
#endif

    return true;

}


Tile * CornerStitching::insertTile(const int x, const int y, const int width, const int height, const int i){
    Tile* t = new Tile(x,y,width,height,i);
    all.insert(t);
    _tile_num++;
    insertTile(t);
    return t;
}


void CornerStitching::topSplit(Tile* space, Tile* block){
    
    if(space->index!=-1){
        std::cerr<<"Error: the tile is not a space."<<std::endl;
        exit(1);
    }
    if(block->topY()==space->topY()){//no need to split
        Tile* temp = space->rt;
        while(temp && temp->x >= block->rightX())//find the right-top tile for block
            temp = temp->bl;
        block->rt = temp;
        updateTopNeighbor(block);
        return;
    }
    //set t2 as the space that doesn't contain block
    Tile* t1 = space;
    Tile* t2 = new Tile(t1); 
    all.insert(t2);
    _tile_num++;
    t2->y = block->topY();
    t2->height = t1->topY() - block->topY();
    t2->lb = t1;
    while(t2->bl && t2->bl->topY() <= t2->y)
       t2->bl = t2->bl->rt;

    updateLeftNeighbor(t2);
    updateTopNeighbor(t2);
    updateRightNeighbor(t2);

    t1->height = block->topY() - t1->y;
#ifdef DEBUG
    if(t1==t2){
        std::cerr<<"Error: t1==t2"<<std::endl;
        std::cerr<<t1->x<<" "<<t1->y<<std::endl;
        exit(1);
    }
#endif
    t1->rt = t2;
    
    while(t1->tr && t1->tr->y >= t1->topY())
       t1->tr = t1->tr->lb;

    block->rt = t2;
}

void CornerStitching::bottomSplit(Tile* space, Tile* block){
    if(space->index!=-1){
        std::cerr<<"Error: the tile is not a space."<<std::endl;
        exit(1);
    }
    if(block->y == space->y){//no need to split
        Tile* temp = space->lb;
        while(temp && temp->rightX() <= block->x)
            temp = temp->tr;
        block->lb = temp;
        updateBottomNeighbor(block);
        return;
    }

    //set t2 as the space that doesn't contain block
    Tile* t1 = space;
    Tile* t2 = new Tile(t1); 
    all.insert(t2);
    _tile_num++;

    t2->height = block->y - t2->y;
#ifdef DEBUG
    if(t1==t2){
        std::cerr<<"Error: t1==t2"<<std::endl;
        std::cerr<<t1->x<<" "<<t1->y<<std::endl;
        exit(1);
    }
#endif
    t2->rt = t1;
    while(t2->tr && t2->tr->y >= t2->topY())
       t2->tr = t2->tr->lb;
    
    updateLeftNeighbor(t2);
    updateBottomNeighbor(t2);
    updateRightNeighbor(t2);

    t1->height = t1->topY() - block->y;
    t1->y = block->y;
    t1->lb = t2;
    while(t1->bl && t1->bl->topY() <= t1->y)
       t1->bl = t1->bl->rt;

    block->lb = t2;

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
    if(!top){//merge the previous tile except the first time
        if(pre_l)
            mergeSpace(pre_l, pre_l->lb);
        if(pre_r)
            mergeSpace(pre_r, pre_r->lb);
        return;
    }
    if(top->index!=-1 || bottom->index!=-1 || block->index==-1){
        std::cerr<<"Error: wrong tile type.(spaceMerge))"<<std::endl;
        exit(1);
    }

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
       
        Tile *temp = top->tr;
        while(temp && temp->bl == top){
            temp->bl = block;
            temp = temp->lb;
        }

        temp = top->bl;
        while(temp && temp->tr == top){
            temp->tr = block;
            temp = temp->rt;
        }

        all.erase(top);
        delete top;
        _tile_num--;

    }else if(r_width == 0){//the right edge of space is aligned with the right edge of block
        if(first)
            block->tr = top->tr;
        if(!next)
            block->bl = top;
        
        Tile *temp = top->tr;
        while(temp && temp->bl == top){
            temp->bl = block;
            temp = temp->lb;
        }

        top->width -= block->width;
        top->tr = block;

        top->rt = pre_l;
        l_top = top;
    }else if(l_width == 0){//the left edge of space is aligned with the right edge of block
        if(first)
            block->tr = top;
        if(!next)
            block->bl = top->bl;
        
        Tile *temp = top->bl;
        while(temp && temp->tr == top){
            temp->tr = block;
            temp = temp->rt;
        }

        top->width -= block->width;
        top->bl = block;
        top->x = block->rightX();
        while(top->lb && top->lb->rightX() <= top->x)
            top->lb = top->lb->tr;
        r_top = top;
    }else{
        r_top = new Tile(top);
        all.insert(r_top);
        _tile_num++;
        l_top = top;
        if(first)
            block->tr = r_top;
        if(!next)
            block->bl = l_top;

        r_top->x = block->rightX();
        r_top->width = top->rightX()-block->rightX();
        r_top->bl = block;
        while(r_top->lb && r_top->lb->rightX() <= r_top->x)
            r_top->lb = r_top->lb->tr;
        
        updateTopNeighbor(r_top);
        updateRightNeighbor(r_top);
        updateBottomNeighbor(r_top);

        l_top->width = block->x - l_top->x;
        l_top->tr = block;
        l_top->rt = pre_l;
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
        return false;
    }
    if(pre->y != cur->topY()){
        return false;
    }
    if(pre->width==cur->width && pre->x == cur->x){
        cur->height += pre->height;
        cur->rt = pre->rt;
        cur->tr = pre->tr;

        updateLeftNeighbor(cur);
        updateRightNeighbor(cur);
        updateTopNeighbor(cur);

        all.erase(pre);
        delete pre;
        _tile_num--;
        return true;
    }else{
        return false;
    }

}

inline void CornerStitching::updateLeftNeighbor(Tile* t, Tile* temp){
    //left
    if(!temp){
        temp = t->bl;
        if(temp && (temp->rightX()!=t->x || temp->topY()<=t->y || temp->y > t->y)){
            std::cerr<<"Error: found wrong pointer when update left."<<std::endl;
            std::cerr<<t->x<<" "<<t->y<<" "<<t->rightX()<<" "<<t->topY()<<" "<<t->index<<std::endl;
            std::cerr<<temp->x<<" "<<temp->y<<" "<<temp->rightX()<<" "<<temp->topY()<<" "<<temp->index<<std::endl;
            exit(1);
        }
    }
    while(temp && temp->topY() <= t->topY()){
        temp->tr = t;
        temp = temp->rt;
    }
}


inline void CornerStitching::updateRightNeighbor(Tile* t, Tile* temp){
    //right
    if(!temp){
        temp = t->tr;
        if(temp && (temp->x!=t->rightX() || temp->topY()<t->topY() || temp->y >= t->topY())){
            std::cerr<<"Error: found wrong pointer when update right."<<std::endl;
            std::cerr<<t->x<<" "<<t->y<<" "<<t->width<<" "<<t->height<<" "<<t->index<<std::endl;
            exit(1);
        }
    }
    while(temp && temp->y >= t->y){
        temp->bl = t;
        temp = temp->lb;
    }
}

inline void CornerStitching::updateTopNeighbor(Tile* t, Tile* temp){
    //top
    if(!temp){
        temp = t->rt;
        if(temp && (temp->y!=t->topY() || temp->x >= t->rightX() || temp->rightX() < t->rightX())){
            std::cerr<<"Error: found wrong pointer when update top."<<std::endl;
            std::cerr<<t->x<<" "<<t->y<<" "<<t->rightX()<<" "<<t->topY()<<" "<<t->index<<std::endl;
            std::cerr<<temp->x<<" "<<temp->y<<" "<<temp->rightX()<<" "<<temp->topY()<<" "<<temp->index<<std::endl;
            exit(1);
        }
    }
    while(temp && temp->x >= t->x){
        temp->lb = t;
        temp = temp->bl;
    }
}


inline void CornerStitching::updateBottomNeighbor(Tile* t, Tile* temp){
    //bottom
    if(!temp){
        temp = t->lb;
        if(temp && (temp->topY()!=t->y || temp->rightX() <= t->x || temp->x > t->x)){
            std::cerr<<"Error: found wrong pointer when update bottom."<<std::endl;
            std::cerr<<t->x<<" "<<t->y<<" "<<t->width<<" "<<t->height<<" "<<t->index<<std::endl;
            exit(1);
        }
    }
    while(temp && temp->rightX() <= t->rightX()){
        temp->rt = t;
        temp = temp->tr;
    }
}


