#ifndef VERLETLEVEL_H
#define VERLETLEVEL_H

#include "engine/common/world.h"

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
//Handles: player, collision (player-verlet), selection, controls

//Controls:
//U: move player back to starting positions
//F: freeze
//LMB: hovering above selectable point
//RMB: freeze/ unfreeze
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
protected:
    Player* _player;
    float _height;
    VerletManager* _vManager;
    ConstraintManager* _cManager;
    Vector3 _startPos;

    //For selection
    bool dragMode = false; //true if player selects point + holds LMB
    //World-space pt: where cursor's ray intersects w/ draggedPoint's plane
    Vector3 draggedMouse;
    Vector3 interpolate;
    float _mouseSpeed;

    Constraint* selectedC=NULL;
    Constraint* hoveredC=NULL;
};

#endif // VERLETLEVEL_H
