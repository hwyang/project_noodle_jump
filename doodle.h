#ifndef DOODLE_H_INCLUDED
#define DOODLE_H_INCLUDED
class Doodle{
public:
    Doodle() {x = 100, y = 100, dy = 0;}

    setPosition(int x, int y){
        this->x = x;
        this->y = y;
    }
    //static bool isOverlap(Circle*, Circle*);
    int x, y;
    bool left_right;
    float dy, init_dy_norm = -10, init_dy_sponge = -20;
};


#endif // DOODLE_H_INCLUDED
