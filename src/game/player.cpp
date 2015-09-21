#include "player.h"
#include "engine/common/camera.h"
#include "engine/common/graphic.h"
#include "engine/common/ellipsoid.h"
#include "engine/common/world.h"
#include "engine/verlet/verletmanager.h"

Player::Player(Camera* c, World* w, VerletManager* v, float height): Entity(),
    _camera(c),
    _world(w),
    _vm(v),
    _playerHeight(height),
    maxVel(15),
    jumpWindow(5),
    jumpVel(7),
    controlScalar(4),
    controlMaxDur(60),
    verletMaxDur(60),
    gravityMaxDur(60)
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
    //Factors for time-based scaling (ease-in/out) of forces upon player motion:
        //Direction of influence (camera orientation for WASD, verlet velocity for friction, etc)
        //Duration of influence (how long player has been holding WASD, been on verlet, etc)

    //As acceleration (and therefore velocity) are composed of forces with changeable factors,
        //they need to be re-calculated per tick based on updates to direction + duration

    //RESET: at beginning, so values can be read by verlet during its update
    _vel = Vector3(0,0,0);
    _acc = Vector3(0,0,0);

    //UPDATE INFLUENCES
    onGround = _mtv.y>0;
    if(onGround&&!onVerlet&&_vm->solve){    //Initial contact w/ verlet: reset duration + direction
        verletDur=0;
        _verletAcc=Vector3(0,0,0);
    }
    if(onGround){                           //On verlet: can jump for the next #jumpDelay ticks
        onVerlet = true;
        jumpCounter = jumpWindow;
    }
    if(!_vm->solve)
        onVerlet = false;

    //Counters for duration
    if(!onGround&&jumpCounter>0)
        jumpCounter--;
    if(jumpCounter<=0)
        onVerlet=false;
    if(onVerlet&&verletDur<verletMaxDur)
        verletDur++;
    else if(!onVerlet&&verletDur>0)
        verletDur--;
    if(!onGround&&gravityDur<gravityMaxDur)
        gravityDur++;
    if(onGround&&gravityDur>gravityMin)
        gravityDur--;

    //CALCULATE COMPONENT FORCES
    //Calculate force from WASD controls
    Vector3 controlForce = Vector3(0,0,0);
    for(int i = 0; i<4; i++){
        Vector3 dir;                            //update direction
        if(i==W)dir = _camera->getForward();
        if(i==A)dir = _camera->getLeft();
        if(i==S)dir = _camera->getBackward();
        if(i==D)dir = _camera->getRight();

        int& dur = controlDur[i];               //update duration
        if(controlOn[i]&&dur<=controlMaxDur)
            dur++;
        else if(dur>0)
            dur--;

        controlForce+=dir*controlScalar*dur;
    }
    _acc += controlForce;

    //Calculate force from verlet friction
    Vector3 frictionForce =  _verletAcc*(1.0/seconds);      //velocity
    frictionForce = frictionForce/seconds;                  //acc
    float fraction = (float)verletDur/verletMaxDur;
    frictionForce = frictionForce*fraction;

    if(onGround)
        _acc += frictionForce;
    else{    //can cause strange 'jumps' if y is applied
        _acc.x+=frictionForce.x;
        _acc.z+=frictionForce.z;
    }

    //Calculate force from gravity
    _acc += _world->getGravity()*gravityDur;

    //VELOCITY
    _vel += _acc*seconds;
    if(jumpVec.y>0){                                //apply jump
        jumpVec.y+=_world->getGravity().y*seconds;
        gravityDur--;
        _vel.y = jumpVec.y;
    }
    if(_vel.lengthSquared()>maxVel*maxVel){         //apply cap
        _vel.normalize();
        _vel*=maxVel;
    }
    _toMove  = seconds*_vel;

    //Update camera
    _camera->moveTo(_shape->getPos()+Vector3(0,_playerHeight,0));
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
    setVerletAcc(Vector3(0,0,0));

    jumpCounter = 0;
    for(int i=0;i<4;i++){
        controlOn[i]=false;
        controlDur[i]=0;
    }
    verletDur = 0;
    gravityDur = 0;
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
        if(event->key() == Qt::Key_Space && onVerlet){
            jumpVec=Vector3(0,jumpVel,0);
            gravityDur=gravityMin;
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
