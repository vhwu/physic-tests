#ifndef LINK_H
#define LINK_H

#include <iostream>

struct Link
{
public:
    //indices of points and length between them
    Link(int a, int b, float length);
    ~Link();

    int pointA = -1;
    int pointB = -1;
    float restLength;
    //precalculated for squareroot approximation
    float restLengthSq;


    friend std::ostream& operator<<(std::ostream& os, const Link& t)
    {
        os << "Link:" << t.pointA << "," << t.pointB;
        return os;
    }

    //bool operator == (const Link &l)
    //const {return pointA==l.pointA&&pointB==l.pointB;}
};

#endif // LINK_H
