#include "verlettest5.h"
#include "engine/verlet/rotationconstraint.h"
#include "engine/verlet/translationconstraint.h"
#include "engine/verlet/angleconstraint.h"
#include "engine/verlet/setupcircle.h"

VerletTest5::VerletTest5(Screen *s):VerletLevel(s)
{
    this->_gravity = Vector3(0,0,0);

    //Verlet variables
    //numVerlets:9 numTriangles:3 dim:50 percent:.5
    _vManager->gravity = Vector3(0,-1.9,0);
    int numVerlets = 12;
    int numTriangles = 6; //on edge of verlet along circle
    Vector2 dim = Vector2(numTriangles+1,20);
    float percentVerlet = .8; //ratio between verlet:space on circle

    //Circle variables
    Axis a = Y;
    float radius = 4;
    Vector3 center = Vector3(0,0,0);

    //Setup circle
    SetupCircle circle = SetupCircle(radius,center,a);
    SetupInfo test = circle.positionVerlets(numVerlets,percentVerlet,numTriangles);

    //Base constraint: control point for player, and referenced by all others for its angle
    Cloth* c = new Cloth(dim, test.triSize, test.startPos[0], a, _vManager, test.angles[0]);
    _vManager->addVerlet(c);
    RotationConstraint* rc = new RotationConstraint(c->getCorner(0),a,center,radius,c,true);
    _cManager->addConstraint(rc);
    _cManager->addConstraint(new RotationConstraint(c->getCorner(1),a,center,radius,c));
    c->createLink(c->getCorner(0),c->getCorner(1));   //to provide offset within the same verlet

    for(int i = 1; i<numVerlets; i++){
        Cloth* v = new Cloth(dim, test.triSize, test.startPos[i], a, _vManager, test.angles[i]);
        _vManager->addVerlet(v);
        AngleConstraint* angle = new AngleConstraint(v->getCorner(0),v,rc);
        RotationConstraint* r = new RotationConstraint(v->getCorner(1),a,center,radius,v);
        TranslationConstraint* t = new TranslationConstraint(v->getCorner(1),Y,5,v,true);

        v->createLink(v->getCorner(0),v->getCorner(1));

        _cManager->addConstraint(angle);
        _cManager->addConstraint(r);
        _cManager->addConstraint(t);

        t->assign(X,r);
        t->assign(Z,r);
        angle->assign(Y,t);
        r->assign(Y,t);
    }
}

VerletTest5::~VerletTest5()
{

}

