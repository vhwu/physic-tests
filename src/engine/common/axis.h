#ifndef AXIS
#define AXIS

#include "vector.h"

enum Axis
{
    X, Y, Z, NO_AXIS
};
const Vector3 axis_coor []={Vector3(1,0,0),Vector3(0,1,0),Vector3(0,0,1)};

//To quickly access the next/prev axis w/o checks
inline int next(Axis a){
    return (a+1+3)%3;
}
inline int prev(Axis a){
    return (a-1+3)%3;
}

inline float clamp(float x, float a, float b){
    return x < a ? a : (x > b ? b : x);
}
#endif // AXIS

