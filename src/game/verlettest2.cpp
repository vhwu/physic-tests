#include "verlettest2.h"
#include "engine/verlet/rotationconstraint.h"
#include "engine/verlet/verletconstraint.h"

#include "engine/verlet/translationconstraint.h"

VerletTest2::VerletTest2(Screen* s): VerletLevel(s){
    //GRAVITY
    _manager->gravity = Vector3(0,-2,0);
    this->_gravity = Vector3(0,0,0);
    _mouseSpeed = .4;

    //VERLETS+CONSTRAINTS
    float yOffset = -3;
    float centerOffset = 1.2;
    float triSize = .3;
    float clothWidth = 9;
    float clothLength = 20;
    float radius = 2;
    float r2 = 2.3; //To have constraints at corners, not center of cloth
    int i2 = 82;

    Cloth* c1 = new Cloth(Vector2(clothWidth,clothLength), triSize, Vector3(-radius,yOffset,centerOffset), Y, _manager);
    _manager->addVerlet(c1);
    _cManager->addConstraint(new RotationConstraint(0, Y, c1->getPoint(4)+Vector3(radius,0,0),r2,c1,true));
    _cManager->addConstraint(new RotationConstraint(8, Y, c1->getPoint(4)+Vector3(radius,0,0),r2,c1,true));
    c1->createLink(0,8);

    Cloth* c2 = new Cloth(Vector2(clothWidth,clothLength), -triSize, Vector3(radius,yOffset,-centerOffset), Y, _manager);
    _manager->addVerlet(c2);
    _cManager->addConstraint(new RotationConstraint(0, Y, c2->getPoint(4)+Vector3(-radius,0,0),r2,c2,true));
    _cManager->addConstraint(new RotationConstraint(8, Y, c2->getPoint(4)+Vector3(-radius,0,0),r2,c2,true));
    c2->createLink(0,8);

    Cloth* c3 = new Cloth(Vector2(clothLength,clothWidth), triSize, Vector3(centerOffset-triSize/2.0,yOffset,-radius), Y, _manager);
    _manager->addVerlet(c3);
    _cManager->addConstraint(new RotationConstraint(0, Y, c3->getPoint(i2)+Vector3(0,0,radius),r2,c3,true));
    _cManager->addConstraint(new RotationConstraint(164, Y, c3->getPoint(i2)+Vector3(0,0,radius),r2,c3,true));
    c3->createLink(0,164);

    Cloth* c4 = new Cloth(Vector2(clothLength,clothWidth), -triSize, Vector3(-centerOffset+triSize/2.0,yOffset,radius), Y, _manager);
    _manager->addVerlet(c4);
    _cManager->addConstraint(new RotationConstraint(0, Y, c4->getPoint(i2)+Vector3(0,0,-radius),r2,c4,true));
    _cManager->addConstraint(new RotationConstraint(164, Y, c4->getPoint(i2)+Vector3(0,0,-radius),r2,c4,true));
    c4->createLink(0,164);

    _cManager->addConstraint(new VerletConstraint(0,c1,0,c2));
    _cManager->addConstraint(new VerletConstraint(0,c1,8,c2));
    _cManager->addConstraint(new VerletConstraint(8,c1,0,c2));
    _cManager->addConstraint(new VerletConstraint(8,c1,8,c2));

    _cManager->addConstraint(new VerletConstraint(0,c3,0,c2));
    _cManager->addConstraint(new VerletConstraint(164,c3,8,c1));
    _cManager->addConstraint(new VerletConstraint(0,c4,0,c1));
    _cManager->addConstraint(new VerletConstraint(164,c4,8,c2));

    _cManager->addConstraint(new VerletConstraint(0,c3,0,c4));
    _cManager->addConstraint(new VerletConstraint(0,c3,164,c4));
    _cManager->addConstraint(new VerletConstraint(164,c3,0,c4));
    _cManager->addConstraint(new VerletConstraint(164,c3,164,c4));
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
