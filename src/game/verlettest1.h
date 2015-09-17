#ifndef VERLETTEST1_H
#define VERLETTEST1_H

#include "verletlevel.h"

//For testing translation constraints for wave propagation
class VerletTest1:  public VerletLevel
{
public:
    VerletTest1(Screen* s);
    virtual ~VerletTest1();
};

#endif // VERLETTEST1_H
