#include "player.h"
#include "engine/common/camera.h"
#include "engine/common/graphic.h"
#include "engine/common/ellipsoid.h"
#include "engine/common/world.h"


Player::Player(Camera* c, World* w, float height): Entity(),
    _camera(c),
    _world(w),
    _playerHeight(height),
    jumpDelay(3),
    jumpAcc(300),
    curveScalar(3),
    curveLength(60),
    maxAcc(300)
{
    _shape = new Ellipsoid(Vector3(0,0,0), Vector3(.5,.5,.5));
}

Player::~Player()
{
    delete _shape;
}

Ellipsoid* Player::getEllipsoid(){
    return (Ellipsoid*) _shape;
}

void Player::onTick(float seconds)
{
    //Check if player is on ground
    //If so, can jump for the next X (jumpDelay) ticks
    onGround = _mtv.y>0;
    if(onGround){
        canJump = true;
        jumpCounter = jumpDelay;
    }
    else if(jumpCounter>0)
        jumpCounter--;
    else
        canJump = false;

    //Calculate horizontal force- from player input + goal velocity
    //Note: kept separate from vertical acc b/c horizontal acc is dependent on camera angle,
        //and therefore has to be overwritten per tick in case player turns
    Vector3 horizontalAcc = Vector3(0,0,0);
    for(int i = 0; i<4; i++){
        Vector3 dir;
        if(i==W)dir = _camera->getForward();
        if(i==A)dir = _camera->getLeft();
        if(i==S)dir = _camera->getBackward();
        if(i==D)dir = _camera->getRight();

        int& goal = controlGoal[i];
        if(controlOn[i]&&goal<=curveLength) //taper on force applied until player approaches goal vel
            goal++;
        else if(goal>0) //taper off force applied after player reaches goal vel
            goal--;
        horizontalAcc+=dir*curveScalar*goal;
    }
    _acc += horizontalAcc;

    //Calculate vertical force (gravity)- applied if player is in the air
    if(!onGround)
        _acc += _world->getGravity();
    if(onGround)
        _acc -= _world->getGravity(); //works better than just not applying gravity

    //Clamp force, so as to not pass through cloth
    _acc.y = clamp(_acc.y,-maxAcc,maxAcc);
    _acc.x = clamp(_acc.x,-maxAcc,maxAcc);
    _acc.z = clamp(_acc.z,-maxAcc,maxAcc);

    //Calculate velocity
    _vel = _acc*seconds;
    Vector3 translate =seconds*_vel + .5f*_acc*seconds*seconds;
    _toMove = translate;

    //Erase horizontal acc, so it can be overwritten next tick
    _acc-=horizontalAcc;

    //Update camera
    _camera->moveTo(_shape->getPos()+Vector3(0,_playerHeight,0));

    //this->move(this->_toMove);  //automatic movement
}

void Player::onCollide(Entity *e, const Vector3& mtv){
    Entity::onCollide(e,mtv);
}

void Player::move(const Vector3& translate){
    _shape->translate(translate);
    _toMove = Vector3(0,0,0);
}

void Player::resetPos(const Vector3& pos){
    setPos(pos);
    setVel(Vector3(0,0,0));
    setAcc(Vector3(0,0,0));
}


void Player::onDraw(Graphic* g){
    g->setColor(Vector4(.8,.8,.8,1));
    Entity::onDraw(g);
}

void Player::keyPressEvent(QKeyEvent *event)
{
        if(event->key() == Qt::Key_W) controlOn[W]=true;
        if(event->key() == Qt::Key_S) controlOn[S]=true;
        if(event->key() == Qt::Key_A) controlOn[A]=true;
        if(event->key() == Qt::Key_D) controlOn[D]=true;
        if(event->key() == Qt::Key_Space && canJump) {
            _acc.y=jumpAcc;
        }
    if(event->key() == Qt::Key_P) _camera->changePerson();
}


void Player::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_W) controlOn[W]=false;
    if(event->key() == Qt::Key_S) controlOn[S]=false;
    if(event->key() == Qt::Key_A) controlOn[A]=false;
    if(event->key() == Qt::Key_D) controlOn[D]=false;
}
