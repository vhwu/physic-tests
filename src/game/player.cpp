#include "player.h"
#include "engine/common/camera.h"
#include "engine/common/graphic.h"
#include "engine/common/ellipsoid.h"


Player::Player(Camera* c, float height): Entity()
{
    _camera = c;
    _playerHeight = height;

    _goal =4; //4;
    _dampen = .95f;
    _jumpVel = 6;

    _w=false;
    _a=false;
    _s=false;
    _d=false;
    _r=false; //up
    _f=false; //down

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
    float gravity = -8;
    _goal =100;

    onGround = _mtv.y>0;

    //velocity along the ground plane
    Vector3 goalV = Vector3(0,0,0);
    if(_w) goalV+=_camera->getForward()*_goal;
    if(_s) goalV+=_camera->getBackward()*_goal;
    if(_a) goalV+=_camera->getLeft()*_goal;
    if(_d) goalV+=_camera->getRight()*_goal;
    if(_r) goalV+=Vector3(0,3.0,0)*_goal;
    if(_f) goalV+=Vector3(0,-1.0,0)*_goal;

    //Force due to player input
//    _acc += goalV-_acc;
    _acc += goalV; //-_acc;


    //Gravity- applied if player is in the air
    if(!onGround)
        _acc.y += gravity;
    if(onGround)
        _acc.y -= gravity; //works better than just not applying gravity

    //Clamp force, so as to not pass through cloth
    float max = 300; //at 400, player passes through cloth
    _acc.y = clamp(_acc.y,-max,max);
    _acc.x = clamp(_acc.x,-max,max);
    _acc.z = clamp(_acc.z,-max,max);

    //Velocity
    _vel = _acc*seconds;
    Vector3 translate =seconds*_vel + .5f*_acc*seconds*seconds;
    _toMove = translate;

    //Erase horizontal
    _acc-=goalV;

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
        if(event->key() == Qt::Key_W) _w=true;
        if(event->key() == Qt::Key_S) _s=true;
        if(event->key() == Qt::Key_A) _a=true;
        if(event->key() == Qt::Key_D) _d=true;
        if(event->key() == Qt::Key_R) _r=true;
        if(event->key() == Qt::Key_F) _f=true;
        if(event->key() == Qt::Key_Space && onGround) {
//            _vel= Vector3(_vel.x,_jumpVel,_vel.z);
            _acc+=Vector3(0,300,0);
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
