#ifndef VERLETTEST5_H
#define VERLETTEST5_H

#include "verletlevel.h"

//Staircase: testing angle + rotation + translation constraints in conjunction
class TranslationConstraint;
class VerletTest5: public VerletLevel
{
public:
    VerletTest5(Screen* s);
    ~VerletTest5();
};

#endif // VERLETTEST5_H
