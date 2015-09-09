#include "player.h"
#include "engine/common/camera.h"
#include "engine/common/graphic.h"
#include "engine/common/ellipsoid.h"


Player::Player(Camera* c, float height): Entity()
{
    _camera = c;
    _playerHeight = height;

    _goal =7; //4;
    _dampen = .95f;
    _jumpVel = 8;

    _w=false;
    _a=false;
    _s=false;
    _d=false;
    _r=false; //up
    _f=false; //down

    _shape = new Ellipsoid(Vector3(0,0,0), Vector3(.7,.7,.7));
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
    //velocity along the ground plane
    Vector3 goalV = Vector3(0,0,0);
    if(_w) goalV+=_camera->getForward()*_goal;
    if(_s) goalV+=_camera->getBackward()*_goal;
    if(_a) goalV+=_camera->getLeft()*_goal;
    if(_d) goalV+=_camera->getRight()*_goal;
    if(_r) goalV+=Vector3(0,3.0,0)*_goal;
    if(_f) goalV+=Vector3(0,-1.0,0)*_goal;

    _acc += goalV-_vel;
    onGround = _mtv.y>0;

    Entity::onTick(seconds);

    this->move(this->_toMove);
}

void Player::onCollide(Entity *e, const Vector3& mtv){
    Entity::onCollide(e,mtv);
}

void Player::move(const Vector3& translate){
    _shape->translate(translate);
    _camera->moveTo(_shape->getPos()+Vector3(0,_playerHeight,0));
    _toMove = Vector3(0,0,0);
}


void Player::onDraw(Graphic* g){
    g->setColor(Vector4(.8,.8,.8,1));
    Entity::onDraw(g);
}

void Player::keyPressEvent(QKeyEvent *event)
{
        if(event->key() == Qt::Key_W) _w=true;
        if(event->key() == Qt::Key_S) _s=true;
        if(event->key() == Qt::Key_A) _a=true;
        if(event->key() == Qt::Key_D) _d=true;
        if(event->key() == Qt::Key_R) _r=true;
        if(event->key() == Qt::Key_F) _f=true;
        if(event->key() == Qt::Key_Space && onGround) {
            _vel= Vector3(_vel.x,_jumpVel,_vel.z);
        }
    if(event->key() == Qt::Key_P) _camera->changePerson();
}


void Player::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_W) _w=false;
    if(event->key() == Qt::Key_S) _s=false;
    if(event->key() == Qt::Key_A) _a=false;
    if(event->key() == Qt::Key_D) _d=false;
    if(event->key() == Qt::Key_R) _r=false;
    if(event->key() == Qt::Key_F) _f=false;
}
