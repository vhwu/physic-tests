#ifndef VERLETTEST3_H
#define VERLETTEST3_H

#include "verletlevel.h"

//For testing angle constraints, and constraints assigning control of certain axis to other constraints
class VerletTest3: public VerletLevel
{
public:
    VerletTest3(Screen* s);
    ~VerletTest3();

    void onTick(float seconds);
    void onDraw(Graphic *g);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    //For dragging
    bool dragMode = false; //true if player selects point + holds LMB
    //Selected attributes- don't change once dragMode is enabled
    int draggedPoint = 0;
    Verlet* draggedVerlet = NULL;
    //For moving the selected point
    //World-space pt: where cursor's ray intersects w/ draggedPoint's plane
    Vector3 draggedMouse;
    Constraint* draggedConstraint;
    //from draggedPoint to draggedMouse
    Vector3 interpolate;
};

#endif // VERLETTEST3_H
