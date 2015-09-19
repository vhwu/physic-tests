#ifndef VERLETTEST2_H
#define VERLETTEST2_H

#include "verletlevel.h"

//Kept as (bad) example of using links to maintain angle offsets- see VerletTest4 for more successful example
//For testing rotation constraints + verlet-verlet constraints for centrifugal/ centripetal force
class SetupCircle;
class VerletTest2:  public VerletLevel
{
public:
    VerletTest2(Screen* s);
    virtual ~VerletTest2();
private:
    SetupCircle* circle;
};

#endif // VERLETTEST2_H
