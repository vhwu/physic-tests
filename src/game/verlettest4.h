#ifndef VERLETTEST4_H
#define VERLETTEST4_H

#include "verletlevel.h"

//For testing set-up utility. Change variables to see effects
//For testing angle constraints.
class SetupCircle;
class VerletTest4: public VerletLevel
{
public:
    VerletTest4(Screen* s);
    ~VerletTest4();

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
private:
    SetupCircle* circle;
};

#endif // VERLETTEST4_H
