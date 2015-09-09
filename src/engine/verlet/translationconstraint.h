#ifndef TRANSLATIONCONSTRAINT_H
#define TRANSLATIONCONSTRAINT_H

#include "verlet.h"

class Graphic;
class TranslationConstraint
{
public:
    TranslationConstraint(int i, Axis a, float _min, float _max, Verlet* verlet);
    TranslationConstraint(int i, Axis a, float range, Verlet* verlet);

    virtual ~TranslationConstraint();

    void constrain();
    void onDraw(Graphic* g);
private:
    float clamp(float value);
    Vector3 matchAxis(float value);

    Axis axis;
    int index;
    Vector3 pinPos;
    float min;
    float max;
    Verlet* v;
};

#endif // TRANSLATIONCONSTRAINT_H
