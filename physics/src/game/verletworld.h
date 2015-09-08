#ifndef VERLETWORLD_H
#define VERLETWORLD_H

#include "engine/common/world.h"
#include "vector.h"
#include "engine/common/raytracer.h"
#include "engine/verlet/link.h"
#include "engine/geometric/triangle.h"


class Player;
class GeometricManager;
class VerletManager;
class OBJ;
class Triangle;
class MeshPath;

class VerletWorld:  public World
{
public:
    VerletWorld(Screen* s);
    ~VerletWorld();

    void onTick(float seconds);
    void onDraw(Graphic *g);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

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

    //For tearing
    bool tearMode = false;
    Link* tearLink;
    int tear_ptA = -1;
    int tear_ptB = -1;
    int tear_prevA = -1;
    int tearA = -1;
    int tearB = -1;
    Verlet* tearVerlet = NULL;
    int tearDelay = 2;
    int tearTimer = 0;

    //for wind
    Vector3 windStartPos;
    Vector3 windEndPos;
    bool windStart = false;
    bool windEnd = false;
    bool windComplete = false;

    bool windConst = false;
private:
    Player* _player;
    float _height;
    VerletManager* _manager;
    GeometricManager* _gManager;
    OBJ* _obj;

    Vector3 windDirection;
};

#endif // VERLETWORLD_H
