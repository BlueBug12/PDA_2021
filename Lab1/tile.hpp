#ifndef TILE_HPP
#define TILE_HPP
struct Tile{
    Tile() = delete;
    Tile(Tile* t){
        *this = *t;
    }
    Tile(const int _x, const int _y, const int w, const int h, const int i){
        x = _x;
        y = _y;
        width = w;
        height = h;
        index = i;
    }
    Tile(Tile const & ) = default;
    Tile(Tile       &&) = default;
    Tile & operator = (Tile const &) = default;
    Tile & operator = (Tile      &&) = default;
    ~Tile() = default;
    
    inline int topY(){
        return y + height;
    }
    inline int rightX(){
        return x + width;
    }

    Tile* rt = NULL;
    Tile* tr = NULL;
    Tile* lb = NULL;
    Tile* bl = NULL;

    int index;// -1 is space
    int x,y;
    int width;
    int height;
};

#endif
