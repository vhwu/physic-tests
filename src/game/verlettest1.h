#ifndef VERLETTEST1_H
#define VERLETTEST1_H

#include "engine/common/world.h"
#include "engine/common/raytracer.h"

class Player;
class VerletManager;
class VerletTest1:  public World
{
public:
    VerletTest1(Screen* s);
    ~VerletTest1();

    void onTick(float seconds);
    void onDraw(Graphic *g);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    //For selection
    HitTest hit;
    RayTracer* _ray;

    //For dragging
    bool dragMode = false; //true if player selects point + holds LMB
    //Selected attributes- don't change once dragMode is enabled
    int draggedPoint = 0;
    Verlet* draggedVerlet = NULL;
    //For moving the selected point
    //World-space pt: where cursor's ray intersects w/ draggedPoint's plane
    Vector3 draggedMouse;
    //from draggedPoint to draggedMouse
    Vector3 interpolate;
private:
    Player* _player;
    float _height;
    VerletManager* _manager;
};

#endif // VERLETTEST1_H