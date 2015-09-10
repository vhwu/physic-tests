#ifndef TRANSLATIONCONSTRAINT_H
#define TRANSLATIONCONSTRAINT_H

#include "verlet.h"

class Graphic;

//Constraint which limits point's movement to a specified axis within a range (min-max)
class TranslationConstraint
{
public:
    TranslationConstraint(int i, Axis a, float _min, float _max, Verlet* verlet);
    TranslationConstraint(int i, Axis a, float range, Verlet* verlet);

    virtual ~TranslationConstraint();

    void constrain();
    //Visualizes range of constraint
    void onDraw(Graphic* g);
private:
    //Returns value clamped within range (min-max)
    float clamp(float value);
    //Given a possible value for the movable axis, returns a valid point
    Vector3 matchAxis(float value);

    Axis axis;
    int index;
    Vector3 pinPos;
    float min;
    float max;
    Verlet* v;
};

#endif // TRANSLATIONCONSTRAINT_H
