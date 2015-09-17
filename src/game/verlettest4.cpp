#include "verlettest4.h"
#include "engine/verlet/rotationconstraint.h"
#include "engine/verlet/angleconstraint.h"
#include "engine/verlet/setupcircle.h"

VerletTest4::VerletTest4(Screen *s): VerletLevel(s){
    //Centered, invisible player
    _startPos = Vector3(0,0,0);
    _player->setPos(_startPos);
    _player->setVisible(false);
    this->_gravity = Vector3(0,0,0);

    //Verlet variables
    //numVerlets:9 numTriangles:3 dim:50 percent:.5
    _vManager->gravity = Vector3(0,-1.9,0);
    int numVerlets = 6;
    int numTriangles = 6; //on edge of verlet along circle
    Vector2 dim = Vector2(numTriangles+1,40);
    float percentVerlet = .7; //ratio between verlet:space on circle

    //Circle variables
    Axis a = Y;
    float radius = 2;
    Vector3 center = Vector3(0,0,0);

    //Setup circle
    circle = new SetupCircle(radius,center,a);
    circle->setAngle(30);

    //Verlets
    SetupInfo test = circle->positionVerlets(numVerlets,percentVerlet,numTriangles);

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
        _cManager->addConstraint(new AngleConstraint(v->getCorner(0),v,rc));
        _cManager->addConstraint(new RotationConstraint(v->getCorner(1),a,center,radius,v));
        v->createLink(v->getCorner(0),v->getCorner(1));
    }
}

VerletTest4::~VerletTest4()
{}

void VerletTest4::onTick(float seconds){
    //NO COLLISIONS- no call to VerletLevel::onTick
    HitTest trace;
    _cManager->rayTrace(_ray,trace);
    hit = trace;

    if(dragMode){
        Vector3 normal = selectedC->getNormal(_camera);
        Verlet* draggedVerlet = selectedC->getVerlet();
        int draggedPoint = selectedC->getIndex();
        draggedMouse = _ray->getPointonPlane(draggedVerlet->getPoint(draggedPoint),normal);
        interpolate = Vector3::lerp(interpolate, draggedMouse, 1 - powf(_mouseSpeed, seconds));
        draggedVerlet->setPos(draggedPoint,interpolate);
    }

    World::onTick(seconds);
}

void VerletTest4::onDraw(Graphic *g){
    //Visualize axes: XYZ = RGB, w/ sphere at 'positive' end
//    Vector3 center = Vector3(0,0,0);
//    float radius = 3;
//    for(int i = 0; i<3; i++){
//        g->setColor(axis_coor[i]);
//        g->drawCircle((Axis)i,radius,center);
//        Vector3 point = Vector3(0,0,0);
//        point.xyz[i]=radius;
//        g->transform(&Graphic::drawUnitSphere,point,0,
//                     Vector3(.1,.1,.1));
//    }

    VerletLevel::onDraw(g);
}

