#ifndef VERLETTEST2_H
#define VERLETTEST2_H

#include "verletlevel.h"

//For testing rotation constraints for centrifugal/ centripetal force
class VerletTest2:  public VerletLevel
{
public:
    VerletTest2(Screen* s);
    virtual ~VerletTest2();

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

#endif // VERLETTEST2_H
