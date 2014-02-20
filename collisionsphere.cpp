#include "collisionsphere.h"

collisionsphere::collisionsphere()
{
    center.x = center.y = center.z = 0;
    radius = 0;
}

// vector3d is the coordinates of the center
collisionsphere::collisionsphere(const vector3d& c, float rad)
{
    center = c;
    radius = rad;
}
