#ifndef NINJA_H_INCLUDED
#define NINJA_H_INCLUDED
#include"doodle.h"

class Ninja : public Doodle{
public:

    int x, y;
    float dy, init_dy_norm = -20, init_dy_sponge = -40;
};


#endif // NINJA_H_INCLUDED
