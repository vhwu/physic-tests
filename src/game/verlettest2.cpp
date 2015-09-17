#include "verlettest2.h"
#include "engine/verlet/rotationconstraint.h"
#include "engine/verlet/verletconstraint.h"
#include "engine/verlet/setupcircle.h"

VerletTest2::VerletTest2(Screen* s): VerletLevel(s){
    //GRAVITY
    _vManager->gravity = Vector3(0,-2,0);
    this->_gravity = Vector3(0,0,0);
    _mouseSpeed = .4;

    //VERLETS+CONSTRAINTS
    //Verlet variables
    int numVerlets = 4;
    int numTriangles = 8; //on edge of verlet along circle
    Vector2 dim = Vector2(numTriangles+1,20);
    float percentVerlet = .5; //ratio between verlet:space on circle
    //Circle variables
    Axis a = Y;
    float radius = 2.3;
    Vector3 center = Vector3(0,-3,0);

    //Setup circle
    circle = new SetupCircle(radius,center,a);

    //Verlets
    SetupInfo test = circle->positionVerlets(numVerlets,percentVerlet,numTriangles);
    for(int i = 0; i<numVerlets; i++){
        Cloth* v = new Cloth(dim, test.triSize, test.startPos[i], a, _vManager, test.angles[i]);
        _vManager->addVerlet(v);
        v->createLink(v->getCorner(0),v->getCorner(1));
        _cManager->addConstraint(new RotationConstraint(v->getCorner(0),a,center,radius,v,true));
        _cManager->addConstraint(new RotationConstraint(v->getCorner(1),a,center,radius,v,true));
    }

    Verlet* c1 = _vManager->getVerlet(0);
    Verlet* c2 = _vManager->getVerlet(1);
    Verlet* c3 = _vManager->getVerlet(2);
    Verlet* c4 = _vManager->getVerlet(3);

    _cManager->addConstraint(new VerletConstraint(0,c1,0,c3));
    _cManager->addConstraint(new VerletConstraint(0,c1,8,c3));
    _cManager->addConstraint(new VerletConstraint(8,c1,0,c3));
    _cManager->addConstraint(new VerletConstraint(8,c1,8,c3));

    _cManager->addConstraint(new VerletConstraint(0,c2,0,c4));
    _cManager->addConstraint(new VerletConstraint(0,c2,8,c4));
    _cManager->addConstraint(new VerletConstraint(8,c2,0,c4));
    _cManager->addConstraint(new VerletConstraint(8,c2,8,c4));

    _cManager->addConstraint(new VerletConstraint(8,c1,0,c2));
    _cManager->addConstraint(new VerletConstraint(8,c2,0,c3));
    _cManager->addConstraint(new VerletConstraint(8,c3,0,c4));
    _cManager->addConstraint(new VerletConstraint(8,c4,0,c1));
}

VerletTest2::~VerletTest2(){}
