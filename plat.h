#ifndef PLAT_H_INCLUDED
#define PLAT_H_INCLUDED

class platform{
public:
    platform() {}

    platform(int x, int y){
        this->x = x;
        this->y = y;
    }
    setPosition(int x, int y){
        this->x = x;
        this->y = y;
    }
    int x, y, plat_type;
};

#endif // PLAT_H_INCLUDED
