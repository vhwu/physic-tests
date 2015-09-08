#ifndef SHEAR_H
#define SHEAR_H

#include "link.h"

//A 'shear constraint (or technically bend constraint)' that crosses over two links
class Bend: public Link
{
public:
    Bend(int a, int b, float length);
    Bend(Link* a, Link* b);
    ~Bend();

    //Arrange these so that the shared point is in the middle
    Link* seg1;
    Link* seg2;
};

#endif // SHEAR_H
