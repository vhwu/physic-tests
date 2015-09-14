#ifndef AXIS
#define AXIS

enum Axis
{
    X, Y, Z, NO_AXIS
};

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

