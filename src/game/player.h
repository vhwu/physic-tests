#ifndef PLAYER_H
#define PLAYER_H

#include <QKeyEvent>
#include "vector.h"
#include "engine/common/entity.h"

class Camera;
class Graphic;
class Ellipsoid;
class World;

enum PlayerControl
{
    W,A,S,D
};

//Controls player movements with WASD
//Include in keyPressEvent and keyReleaseEvent
class VerletManager;
class Player: public Entity
{
public:
    Player(Camera* c, World* w, VerletManager *v, float height);
    ~Player();

    Ellipsoid* getEllipsoid();

    void onTick(float seconds);
    void onDraw(Graphic* g);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void move(const Vector3& translate);    //Updates pos by _toMove, and resets _toMove
    void onCollide(Entity *e, const Vector3 &mtv);
    void resetPos(const Vector3 &pos);      //Sets pos and cancels out any velocity/ acceleration

    void setJump(int j){jumpVel=j;}
    void setContScalar(int s){controlScalar=s;}
    void setContMaxDur(int l){controlMaxDur = l;}
    void setVerMaxDur(int v){verletMaxDur = v;}
    void setMaxVel(int v){maxVel=v;}
private:
    Camera* _camera;
    World* _world;
    VerletManager* _vm;

    float _playerHeight;
    int maxVel;                 //To prevent phasing through cloth
    bool onGround = false;

    //Jumping
    int jumpWindow;             //#ticks player can jump for, after contact w/ ground
    int jumpCounter=0;
    int jumpVel;
    Vector3 jumpVec=Vector3(0,0,0);

    //WASD
    bool controlOn [4]{false};  //Which WASD keys are on
    int controlScalar;          //Constant scalar for force from controls
    int controlMaxDur;          //#ticks for control acceleration/deceleration
    int controlDur [4]{0};      //#ticks player has been pressing controls

    //Friction
    bool onVerlet=false;
    int verletMaxDur;           //#ticks for friction acceleration/deceleration
    int verletDur=0;              //#ticks player has been on verlet

    //Gravity
    int gravityMaxDur;          //#ticks for gravity acceleration
    int gravityMin=1;             //Min gravity- factors into player influence on terrain
    int gravityDur=0;
};

#endif // PLAYER_H
