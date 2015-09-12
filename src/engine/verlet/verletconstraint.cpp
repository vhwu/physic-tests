#include "verletconstraint.h"

VerletConstraint::VerletConstraint(int i1, Verlet* verlet1, int i2, Verlet* verlet2, bool s):
    Constraint(i1,NO_AXIS,verlet1,s)
{
    v2 = verlet2;
    index2 = i2;

    restLength = (v2->getPoint(index2)-v->getPoint(index)).length();
    restLengthSq = restLength*restLength;}

VerletConstraint::~VerletConstraint()
{

}

void VerletConstraint::constrain(){
    Vector3 posA = v->getPoint(index);
    Vector3 posB = v2->getPoint(index2);

    Vector3 delta = posB-posA;
    //the closer delta.dot(delta) is to restLengthSq, the smaller the displacement
    delta*=restLengthSq / (delta.dot(delta)+restLengthSq) - .5;
    posA -= delta;
    posB += delta;

    v->setPos(index,posA);
    v2->setPos(index2,posB);
}

void VerletConstraint::onDraw(Graphic* g){
    g->drawLine(v->getPoint(index),v2->getPoint(index2));
}
