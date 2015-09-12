#ifndef TRANSLATIONCONSTRAINT_H
#define TRANSLATIONCONSTRAINT_H

#include "verlet.h"
#include "constraint.h"

class Graphic;

//Constraint which limits point's movement to a specified axis within a range (min-max)
class TranslationConstraint: public Constraint
{
public:
    TranslationConstraint(int i, Axis a, float _min, float _max, Verlet* verlet, bool s=false);
    TranslationConstraint(int i, Axis a, float range, Verlet* verlet, bool s=false);

    virtual ~TranslationConstraint();

    void constrain();
    //Visualizes range of constraint
     void onDraw(Graphic* g);
private:
    //Returns value clamped within range (min-max)
    float clamp(float value);
    //Given a possible value for the movable axis, returns a valid point
    Vector3 matchAxis(float value);

    Vector3 pinPos;
    float min;
    float max;
};

#endif // TRANSLATIONCONSTRAINT_H
