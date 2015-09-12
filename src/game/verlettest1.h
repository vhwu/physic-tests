#ifndef VERLETTEST1_H
#define VERLETTEST1_H

#include "verletlevel.h"

//For testing translation constraints for wave propagation
class VerletTest1:  public VerletLevel
{
public:
    VerletTest1(Screen* s);
    virtual ~VerletTest1();

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

#endif // VERLETTEST1_H
