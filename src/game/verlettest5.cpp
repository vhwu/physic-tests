#include "verlettest5.h"
#include "engine/verlet/rotationconstraint.h"
#include "engine/verlet/translationconstraint.h"
#include "engine/verlet/angleconstraint.h"
#include "engine/verlet/alignconstraint.h"
#include "engine/verlet/setupcircle.h"

VerletTest5::VerletTest5(Screen *s):VerletLevel(s)
{
    this->_gravity = Vector3(0,0,0);

    //SETUP
    //Verlet variables
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

    //BASE VERLETS
    //Base rotation constraint: control point for player, and referenced by all others for its angle
    Cloth* c = new Cloth(dim, test.triSize, test.startPos[0], a, _vManager, test.angles[0]);
    _vManager->addVerlet(c);
    RotationConstraint* rc = new RotationConstraint(c->getCorner(0),a,center,radius,c,true);
    _cManager->addConstraint(rc);
    RotationConstraint* rPassive = new RotationConstraint(c->getCorner(1),a,center,radius,c);
    _cManager->addConstraint(rPassive);
    c->createLink(c->getCorner(0),c->getCorner(1));   //to provide offset within the same verlet

    //Base translation constraint: control point for player, and referenced by all others for height
    TranslationConstraint* heightControl = new TranslationConstraint(c->getCorner(1),Y,5,c,true,BOT);
    _cManager->addConstraint(heightControl);
    heightControl->assign(X,rPassive);
    heightControl->assign(Z,rPassive);
    rPassive->assign(Y,heightControl);
    rc->assign(Y,heightControl);

    //Utility for staggering height automatically
    AlignConstraint* align = new AlignConstraint(numVerlets-1, heightControl);
    _cManager->addConstraint(align);

    //DEPENDENT VERLETS
    for(int i = 1; i<numVerlets; i++){
        Cloth* v = new Cloth(dim, test.triSize, test.startPos[i], a, _vManager, test.angles[i]);
        _vManager->addVerlet(v);

        //For automating angles
        AngleConstraint* angle = new AngleConstraint(v->getCorner(0),v,rc);
        _cManager->addConstraint(angle);

        RotationConstraint* r = new RotationConstraint(v->getCorner(1),a,center,radius,v);
        r->setVisRange(false);
        _cManager->addConstraint(r);

        v->createLink(v->getCorner(0),v->getCorner(1));

        r->assign(Y,align->access(i-1));
        angle->assign(Y,align->access(i-1));
    }
}

VerletTest5::~VerletTest5()
{

}

