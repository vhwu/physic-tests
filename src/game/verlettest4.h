#ifndef VERLETTEST4_H
#define VERLETTEST4_H

#include "verletlevel.h"

//For testing set-up utility and angle constraints. Change variables to see effects
//Note: player is invisible + non-collidable in this level
class SetupCircle;
class VerletTest4: public VerletLevel
{
public:
    VerletTest4(Screen* s);
    ~VerletTest4();

    void onTick(float seconds);
    void onDraw(Graphic *g);
private:
    SetupCircle* circle;
};

#endif // VERLETTEST4_H
