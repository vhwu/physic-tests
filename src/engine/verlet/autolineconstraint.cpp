#include "autolineconstraint.h"

AutoLineConstraint::AutoLineConstraint(int i, Axis a, float range,
                                       float sp, Verlet* verlet, Position p):
    TranslationConstraint(i, a, range, verlet, false, p)
{
    speed = sp;
    translation = currentPos.xyz[axis];
}

AutoLineConstraint::~AutoLineConstraint()
{

}

void AutoLineConstraint::constrain(){
    float& t = currentPos.xyz[axis];
    if((speed>0&&t>max) ||(speed<0&&t<min)) //reverse direction if you hit min/max
        speed*=-1;
    t+=speed;

    //For other two axes: solve, defer solving, or freeze if specified axis is selected
    Vector3 p = v->getPoint(index);
    currentPos.xyz[nextA]=solveAxis((Axis)nextA,p.xyz[nextA]);
    currentPos.xyz[prevA]=solveAxis((Axis)prevA,p.xyz[prevA]);

    v->setPos(index,currentPos);
}
