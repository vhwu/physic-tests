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
    jumpDelay(3),
    jumpVel(7),
    normalForceScalar(5),
    maxVel(15),
    curveScalar(4),
    curveLength(60),
    verletLength(60)
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
    _vel.x=0;
    _vel.z=0;

    //Check if player is on ground
    //If so, can jump for the next X (jumpDelay) ticks
    onGround = _mtv.y>0;
    if(onGround&&!onVerlet&&_vm->solve){
        verletCounter=0;
        verletAcc=Vector3(0,0,0);
    }
    if(onGround){
        canJump = true;
        onVerlet = true;
        jumpCounter = jumpDelay;
    }
    else if(jumpCounter>0)
        jumpCounter--;
    else{
        canJump = false;
//        verletAcc=Vector3(0,0,0);
        onVerlet=false;
    }
    if(!_vm->solve)
        onVerlet = false;

    if(onVerlet&&verletCounter<verletLength){
        verletCounter++;
    }
    else if(!onVerlet&&verletCounter>0){
        verletCounter--;
    }
//    std::cout<<verletCounter<<std::endl;

    _acc = Vector3(0,0,0);

    //Calculate horizontal force- from player input + goal velocity
    //Note: kept separate from vertical acc b/c horizontal acc is dependent on camera angle,
        //and therefore has to be overwritten per tick in case player turns
    Vector3 horizontalAcc = Vector3(0,0,0);
    for(int i = 0; i<4; i++){
        Vector3 dir;
        if(i==W)dir = _camera->getForward();
        if(i==A)dir = _camera->getLeft();
//        if(i==A)dir = Vector3(0,-1,0);
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

    //Verlet influence test
    Vector3 testAcc =  verletAcc*(1.0/seconds); //velocity
    testAcc = testAcc/seconds; //acc
    float fraction = (float)verletCounter/verletLength;
    testAcc = testAcc*fraction;
    _acc += testAcc;

    //Calculate vertical force (gravity)- applied if player is in the air
/*    if(onVerlet&&!onGround){
        Vector3 m = _mtv;
        if(m.lengthSquared()>0)
            m.normalize();
        _acc += m*normalForceScalar;
        _acc += _world->getGravity();
    }
    else */
//    if(onVerlet){
    if(onGround){

        Vector3 m = _mtv;
        Vector3 a = _acc;
        if(m.lengthSquared()>0)
            m.normalize();
        if(a.lengthSquared()>0)
            a.normalize();
//        if(a.lengthSquared()>0&&m.lengthSquared()>0)
//            std::cout<<m.dot(a)<<std::endl;
        m = m*.5+Vector3(0,.5,0);
//        _acc += m*normalForceScalar*.5;
//        std::cout<<m*normalForceScalar;
        _acc += _world->getGravity()*-.4;
    }
//    if(!onGround)
    else
        _acc += _world->getGravity();

    //************current test
//    if(!onGround)
//        _acc += _world->getGravity();

//    Vector3 m = _mtv;
//    Vector3 a = _acc;
//    if(m.lengthSquared()>0)
//        m.normalize();
//    if(a.lengthSquared()>0)
//        a.normalize();
//    float normalForce = 0;
//    if(a.lengthSquared()>0&&m.lengthSquared()>0){
//        normalForce = m.dot(a);
////        std::cout<<m.dot(a)<<std::endl;
//    }

//    if(normalForce<-.5){
////        _acc += _world->getGravity()*-.4;
//        _acc += _acc*-.4;
//        std::cout<<normalForce<<std::endl;
//    }


//    if(onGround){
//        Vector3 m = _mtv;
//        m.normalize();
//        _acc += m*normalForceScalar;
////        _acc -= _world->getGravity()*normalForceScalar; //.4
//        }

    //Calculate velocity
    _vel += _acc*seconds;
//    _vel+=testAcc*seconds;



    //Cap vel
    if(_vel.length()>maxVel){
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
        if(event->key() == Qt::Key_Space && canJump)
            _vel.y=jumpVel;
    if(event->key() == Qt::Key_P) _camera->changePerson();
}


void Player::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_W) controlOn[W]=false;
    if(event->key() == Qt::Key_S) controlOn[S]=false;
    if(event->key() == Qt::Key_A) controlOn[A]=false;
    if(event->key() == Qt::Key_D) controlOn[D]=false;
}
