#ifndef VERLETLEVEL_H
#define VERLETLEVEL_H

#include "engine/common/world.h"
#include "engine/common/raytracer.h"

#include "engine/common/camera.h"
#include "player.h"
#include "engine/common/graphic.h"
#include "engine/verlet/verletmanager.h"
#include "engine/verlet/constraintmanager.h"
#include "engine/verlet/cloth.h"

class Player;
class VerletManager;
class ConstraintManager;
class Constraint;

//Base Verlet level.
//Handles: updating verlet, constraints, player, collision, raytracing for selection, camera controls

//Controls:
//U: move player back to starting positions
//F: freeze
//LMB: hovering above selectable point, double click for freeze
class VerletLevel:  public World
{
public:
    VerletLevel(Screen* s);
    virtual ~VerletLevel();

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
protected:
    Player* _player;
    float _height;
    VerletManager* _manager;
    ConstraintManager* _cManager;
    Vector3 _startPos;

    float _mouseSpeed;
};

#endif // VERLETLEVEL_H
