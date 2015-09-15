#include "verlettest3.h"
#include "engine/verlet/rotationconstraint.h"
#include "engine/verlet/translationconstraint.h"
#include "engine/verlet/angleconstraint.h"

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

    Cloth* c1 = new Cloth(Vector2(clothWidth,clothLength), triSize, Vector3(-radius,yOffset,centerOffset), Y, _manager);
    _manager->addVerlet(c1);
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

    Cloth* c2 = new Cloth(Vector2(clothWidth,clothLength), triSize, Vector3(2,2,2), Y, _manager);
    _manager->addVerlet(c2);
    c2->visualize = true;

    c2->printCorners();
}

VerletTest3::~VerletTest3()
{}

void VerletTest3::onTick(float seconds){
    //dragging
    if(dragMode&&_manager->solve){
        draggedMouse = _ray->getPointonPlane(draggedVerlet->getPoint(draggedPoint),-1*_camera->getLook());
        interpolate = Vector3::lerp(interpolate, draggedMouse, 1 - powf(_mouseSpeed, seconds));
        draggedVerlet->setPos(draggedPoint,interpolate);
    }
    VerletLevel::onTick(seconds);
}

void VerletTest3::onDraw(Graphic *g){
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
void VerletTest3::mousePressEvent(QMouseEvent *event){
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

void VerletTest3::mouseMoveEvent(QMouseEvent *event){
    VerletLevel::mouseMoveEvent(event);
}

//Set if constraint is deselected
void VerletTest3::mouseReleaseEvent(QMouseEvent *event){
    VerletLevel::mouseReleaseEvent(event);
    if(event->button() == Qt::LeftButton){
        dragMode = false;
        _cManager->resetSelection();
    }
}

