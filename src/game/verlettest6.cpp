#include "verlettest6.h"
#include "engine/verlet/setupcircle.h"
#include "engine/verlet/rotationconstraint.h"
#include "engine/verlet/autorotconstraint.h"
#include "engine/verlet/autolineconstraint.h"


VerletTest6::VerletTest6(Screen *s):
    VerletLevel(s)
{
//    this->_gravity = Vector3(0,0,0);
    _startPos = Vector3(0,6,0);
    _player->setPos(_startPos);

    //SETUP
    //Verlet variables
    _vManager->gravity = Vector3(0,-1.9,0);
    int numVerlets = 5;
    int numTriangles = 12; //on edge of verlet along circle
    Vector2 dim = Vector2(numTriangles+1,20);
    float percentVerlet = .8; //ratio between verlet:space on circle

    //Circle variables
    Axis a = Y;
    float radius = 4;
    Vector3 center = Vector3(0,0,0);

    //Setup circle
    SetupCircle circle = SetupCircle(radius,center,a);
    SetupInfo test = circle.positionVerlets(numVerlets,percentVerlet,numTriangles);

    //Verlets
    Cloth* c = new Cloth(dim, test.triSize, test.startPos[0], a, _vManager, test.angles[0]);
    _vManager->addVerlet(c);  

    AutoRotConstraint* r1 = new AutoRotConstraint(c->getCorner(0),a,center,radius,.1,c);
    _cManager->addConstraint(r1);
    AutoRotConstraint* r2 = new AutoRotConstraint(c->getCorner(1),a,center,radius,.1,c);
    _cManager->addConstraint(r2);

    //Outer circle
    float r = c->getPoint(c->getCorner(2)).length();
    _cManager->addConstraint(new RotationConstraint(c->getCorner(2),a,center,r,c));
    _cManager->addConstraint(new RotationConstraint(c->getCorner(3),a,center,r,c));

    //    AutoLineConstraint* l = new AutoLineConstraint(c->getCorner(0),a,5,.01,c);
    //    _cManager->addConstraint(l);
//    l->assign(X,r1);
//    l->assign(Z,r1);
//    r1->assign(Y,l);
//    r2->assign(Y,l);

    float size = .3;
    Cloth* start = new Cloth(Vector2(20,20), size, Vector3(-1,3,-1), Y, _vManager);
    start->pinCorners();
    _vManager->addVerlet(start);
}

VerletTest6::~VerletTest6()
{

}

