#include "verlettest3.h"
#include "engine/verlet/rotationconstraint.h"
#include "engine/verlet/translationconstraint.h"

VerletTest3::VerletTest3(Screen *s): VerletLevel(s){
    //GRAVITY
    //_manager->gravity = Vector3(0,-2,0);
    this->_gravity = Vector3(0,0,0);
    _mouseSpeed = .4;

    //VERLETS+CONSTRAINTS
    float yOffset = -3;
    float centerOffset = 1.2;
    float triSize = .3;
    float clothWidth = 9;
    float clothLength = 20;
    float radius = 2;
    float r2 = 2.3; //2.3 for constraints- have them at corners, not center of cloth
    int angle = 90;

    Cloth* c1 = new Cloth(Vector2(clothWidth,clothLength), triSize,
                          Vector3(-radius,yOffset,centerOffset), Y, _vManager, angle);
    _vManager->addVerlet(c1);
    RotationConstraint* rControl = new RotationConstraint(0, Y, c1->getPoint(4)+Vector3(radius,0,0),r2,c1,true);
    RotationConstraint* rPassive = new RotationConstraint(8, Y, c1->getPoint(4)+Vector3(radius,0,0),r2,c1);
    TranslationConstraint* t = new TranslationConstraint(8,Y,30,c1,true);

    _cManager->addConstraint(t);

    _cManager->addConstraint(rPassive);
    _cManager->addConstraint(rControl);

    c1->createLink(0,8);

    t->assign(X,rPassive);
    t->assign(Z,rPassive);
    rPassive->assign(Y,t);
    rControl->assign(Y,t);

    //Visualization example
    Cloth* c2 = new Cloth(Vector2(clothWidth,clothLength), triSize, Vector3(2,2,2), Y, _vManager, angle);
    _vManager->addVerlet(c2);
    c2->visualize = true;

    c2->printCorners();
}

VerletTest3::~VerletTest3()
{}

//void VerletTest3::onTick(float seconds){


