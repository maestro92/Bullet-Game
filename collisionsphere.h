#ifndef collisionplane_H
#define collisionplane_H
#include <iostream>
#include "vector3d.h"

class collisionsphere
{
    public:
        float radius;
        vector3d center;
        collisionsphere();
        collisionsphere(const vector3d& vec, float rad);


};

#endif
