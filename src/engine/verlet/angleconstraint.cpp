#include "angleconstraint.h"
#include "rotationconstraint.h"
#include <math.h>

#define PI 3.14159265

AngleConstraint::AngleConstraint(int i, Verlet* verlet, RotationConstraint* r, bool s):
    Constraint(i,r->getAxis(),verlet,s)
{
    rc = r;
    control[axis]=&rc->access(axis); //by default, match unspecified axis w/ that of rc
    currentPos = verlet->getPoint(index);

    //Calculate angle between two points
    Vector3 p1_3 = v->getPoint(i);
    Vector3 p2_3 = rc->getPos();
    Vector2 p1 = Vector2(p1_3.xyz[prevA],p1_3.xyz[nextA]);
    Vector2 p2 = Vector2(p2_3.xyz[prevA],p2_3.xyz[nextA]);

    angle = atan((p1.y - p2.y) / (p2.x - p1.x)) * (180 / PI);
    angle*=2; //not sure why this is necessary
}

AngleConstraint::~AngleConstraint()
{}

void AngleConstraint::constrain(){
    Vector3 center = rc->getCenter();
    Vector3 control = rc->getPos();

    //Find angle constrained point needs to be at, then the point there
    float baseAngle = atan2(control.xyz[prevA] - center.xyz[prevA],
                            control.xyz[nextA] - center.xyz[nextA]);  //x and y switched?
    baseAngle *= (180 / PI);                //Angle of control
    float finalAngle = baseAngle + angle;   //Offset it
    Vector3 point = getPoint(finalAngle);

    //For non-specified axis: solve it, or defer solving to another constraint
    point.xyz[axis] = solveAxis(axis,point.xyz[axis]);

    v->setPos(index,point);
    currentPos = point;
}

Vector3 AngleConstraint::getPoint(float angle){
    Vector3 center = rc->getCenter();
    Vector3 point = center;
    float radius = rc->getRadius();
    point.xyz[prevA] = center.xyz[prevA]+(radius*sin(angle*PI/180));
    point.xyz[nextA] = center.xyz[nextA]+(radius*cos(angle*PI/180));
    return point;
}

void AngleConstraint::onDraw(Graphic* g){
    g->setColor(Vector3(1,1,1));
    Constraint::onDraw(g);
}
