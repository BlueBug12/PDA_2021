#ifndef CORNER_STITCHING_HPP 
#define CORNER_STITCHING_HPP
//#define DEBUG
#include <vector>
#include <iostream>
#include "tile.hpp"

inline int inTileH(Tile & t, const int x){
    if(x >= t.x && x <= t.rightX())//x is in the range of tile t
        return 0;
    else if(x < t.x)//the position of tile t is at right hand side of x
        return -1;
    else//the position of tile t is at left hand side of x
        return 1;
}

inline int inTileV(Tile & t, const int y){
    if(y >= t.y && y <= t.topY())//y is in the range of the tile t
        return 0;
    else if(y < t.y)//the position of tile t is higher than y
        return -1;
    else//the position of tile t is lower than y
        return 1;
}



class CornerStitching{
    public:
        CornerStitching(int width, int height);
        CornerStitching(CornerStitching const & ) = default;
        CornerStitching(CornerStitching &&) = default;
        CornerStitching & operator = (CornerStitching const &) = default;
        CornerStitching & operator = (CornerStitching &&) = default;
        ~CornerStitching() = default;
        Tile* findPoint(int x, int y, Tile *hint =NULL);
        void countNeighbor(Tile* t, int& space_num, int& block_num);        
        bool searchArea(int left_x, int bottom_y, int width, int height);
        bool insertTile(Tile* t);
        Tile * insertTile(const int x, const int y, const int width, const int height, const int i = -1);
        void insertTile(Tile* top, Tile* bottom, Tile* block, Tile* pre_l, Tile* pre_r);
        std::vector<Tile> collectAllTiles();
        void vSplit(Tile & t);
        void topSplit(Tile* space, Tile* block);
        void bottomSplit(Tile* space, Tile* block);
        Tile* searchH(Tile* t_ptr, const int x);
        Tile* searchV(Tile* t_ptr, const int y);
        inline bool crossOverlapping(Tile* space, Tile* block);
        bool mergeSpace(Tile* pre, Tile* cur);

        inline void updateLeftNeighbor(Tile* t, Tile* temp = NULL);
        inline void updateRightNeighbor(Tile* t, Tile* temp = NULL);
        inline void updateTopNeighbor(Tile* t, Tile* temp = NULL);
        inline void updateBottomNeighbor(Tile* t, Tile* temp = NULL);
        void updateAllNeighbor(Tile* t);
        int getTileNum(){return _tile_num;}

    private:
        const int _width;
        const int _height;
        int _tile_num;
        Tile* _start_tile;

        std::vector<Tile*>tiles;
};

#endif
