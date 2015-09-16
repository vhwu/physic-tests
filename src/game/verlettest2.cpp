#include "verlettest2.h"
#include "engine/verlet/rotationconstraint.h"
#include "engine/verlet/verletconstraint.h"
#include "engine/verlet/setupcircle.h"

VerletTest2::VerletTest2(Screen* s): VerletLevel(s){
    //GRAVITY
    _manager->gravity = Vector3(0,-2,0);
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
        Cloth* v = new Cloth(dim, test.triSize, test.startPos[i], a, _manager, test.angles[i]);
        _manager->addVerlet(v);
        v->createLink(v->getCorner(0),v->getCorner(1));
        _cManager->addConstraint(new RotationConstraint(v->getCorner(0),a,center,radius,v,true));
        _cManager->addConstraint(new RotationConstraint(v->getCorner(1),a,center,radius,v,true));
    }

    Verlet* c1 = _manager->getVerlet(0);
    Verlet* c2 = _manager->getVerlet(1);
    Verlet* c3 = _manager->getVerlet(2);
    Verlet* c4 = _manager->getVerlet(3);

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

void VerletTest2::onTick(float seconds){
    //dragging
    if(dragMode&&_manager->solve){
        //Test: use axis of constraint to determine normal for plane that ray is intersected w/, for mouse pos
        Vector3 normal;
        if(draggedConstraint->getAxis()==X)
            normal = Vector3(1,0,0);
        else if(draggedConstraint->getAxis()==Y)
            normal = Vector3(0,1,0);
        else
            normal = Vector3(0,0,1);
        draggedMouse = _ray->getPointonPlane(draggedVerlet->getPoint(draggedPoint),normal);

        //draggedMouse = _ray->getPointonPlane(draggedVerlet->getPoint(draggedPoint),-1*_camera->getLook());
        interpolate = Vector3::lerp(interpolate, draggedMouse, 1 - powf(_mouseSpeed, seconds));
        draggedVerlet->setPos(draggedPoint,interpolate);
    }
    VerletLevel::onTick(seconds);
}

void VerletTest2::onDraw(Graphic *g){
    //VISUALIZATION
    //Interpolation
    if(dragMode){
        //draw mouse pos in cyan
        g->setColor(Vector3(0,1,1));
        g->transform(&Graphic::drawUnitSphere,draggedMouse,0,
                     Vector3(.1,.1,.1));
        //draw interpolated position in green
        g->setColor(Vector3(0,1,0));
        g->transform(&Graphic::drawUnitSphere,interpolate,0,
                     Vector3(.1,.1,.1));
    }
    VerletLevel::onDraw(g);
}

//Set if constraint is manipulated
void VerletTest2::mousePressEvent(QMouseEvent *event){
    VerletLevel::mousePressEvent(event);

    if(event->button() == Qt::LeftButton&& hit.hit){
        _cManager->setSelection(hit.c);
        dragMode = true;
        draggedPoint = hit.id;
        draggedVerlet = hit.v;
        draggedConstraint = hit.c;
        interpolate = hit.v->getPoint(hit.id);
    }
}

void VerletTest2::mouseMoveEvent(QMouseEvent *event){
    VerletLevel::mouseMoveEvent(event);
}

//Set if constraint is deselected
void VerletTest2::mouseReleaseEvent(QMouseEvent *event){
    VerletLevel::mouseReleaseEvent(event);
    if(event->button() == Qt::LeftButton){
        dragMode = false;
        _cManager->resetSelection();
    }
}
