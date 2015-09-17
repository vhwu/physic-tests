#ifndef VERLETTEST3_H
#define VERLETTEST3_H

#include "verletlevel.h"

//For testing angle constraints, and constraints assigning control of certain axis to other constraints
class VerletTest3: public VerletLevel
{
public:
    VerletTest3(Screen* s);
    ~VerletTest3();
};

#endif // VERLETTEST3_H
