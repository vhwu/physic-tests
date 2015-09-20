#ifndef PLAYER_H
#define PLAYER_H

#include <QKeyEvent>
#include "vector.h"
#include "engine/common/entity.h"
#include "engine/common/raytracer.h"

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
class Player: public Entity
{
public:
    Player(Camera* c, World* w, float height);
    ~Player();

    Ellipsoid* getEllipsoid();

    void onTick(float seconds);
    void onDraw(Graphic* g);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    //Updates position of player by _toMove, and resets _toMove
    void move(const Vector3& translate);
    void onCollide(Entity *e, const Vector3 &mtv);
    void resetPos(const Vector3 &pos); //Sets pos and cancels out any velocity/ acceleration

    void setJump(int j){jumpVel=j;}
    void setCurveScalar(int s){curveScalar=s;}
    void setCurveLength(int l){curveLength = l;}
    void setMaxVel(int v){maxVel=v;}
    void setNormalScalar(float n){normalForceScalar=n;}
private:
    Camera* _camera;
    World* _world;
    float _playerHeight;

    bool onGround = false;
    bool canJump = false;
    int jumpCounter;  //for jumpDelay
    int jumpDelay; //number of ticks player can jump for, after contact w/ ground
    int jumpVel;

    float normalForceScalar; //determines fraction of force applied upon contact w/ surface
    int maxVel; //acc cap to prevent phasing through cloth
    bool controlOn [4]{false}; //WASD

    //Goal velocity
    int controlGoal [4]{0}; //per-control counter for acceleration/ deceleration towards goal velocity
    int curveScalar; //rate of acceleration/ deceleration
    int curveLength; //how long the acceleration/ deceleration phase is
};

#endif // PLAYER_H
