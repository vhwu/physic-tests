#include "rotationconstraint.h"

RotationConstraint::RotationConstraint(int i, Axis a, const Vector3& c, float r,
                                       Verlet* verlet,  bool s):
    Constraint(i,a,verlet,s)
{
    circle = new Circle(r,c,a);
    currentPos = v->getPoint(i);

    //By default, point the axis the player doesn't control to center
    control[a]=&circle->getCenter().xyz[a];
}

RotationConstraint::~RotationConstraint()
{
    delete circle;
}

Vector3 RotationConstraint::getNormal(Camera*){
    return axis_coor[axis];
}

void RotationConstraint::constrain(){
    Vector3 p = v->getPoint(index);

    //For non-specified axis: solve it, or defer solving to another constraint
    p.xyz[axis] = solveAxis(axis,p.xyz[axis]);
    circle->setCenter(axis,p.xyz[axis]);

    //For two axes: ensure they're on the circumference of the rotation's circle
    Vector3 onCircle = circle->onCircumference(p);

    v->setPos(index,onCircle);
    currentPos = onCircle;
}

void RotationConstraint::onDraw(Graphic* g){
    //Arc: gray if unselectable, teal if selectable, yellow if selected
    if(selected)
        g->setColor(Vector3(1,1,0));
    else if(selectable)
        g->setColor(Vector3(0,1,1));
    else
        g->setColor(Vector3(.5,.5,.5));
    circle->onDraw(g);
    Constraint::onDraw(g);
}
