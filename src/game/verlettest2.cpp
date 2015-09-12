#include "verlettest2.h"
#include "engine/verlet/rotationconstraint.h"
#include "engine/verlet/verletconstraint.h"

VerletTest2::VerletTest2(Screen* s): VerletLevel(s){
    //GRAVITY
    _manager->gravity = Vector3(0,-2,0);

    //VERLETS+CONSTRAINTS
    float yOffset = -3;
    float centerOffset = 1.2;
    float triSize = .3;
    float clothWidth = 9;
    float clothLength = 20;
    float radius = 2;
    float r2 = 2; //2.3 for constraints- have them at corners, not center of cloth

    Cloth* start = new Cloth(Vector2(12,12), .3, Vector3(0,0,0), Y, _manager);
    start->pinCorners();
    _manager->addVerlet(start);

    Cloth* c1 = new Cloth(Vector2(clothWidth,clothLength), triSize, Vector3(-radius,yOffset,centerOffset), Y, _manager);
    _manager->addVerlet(c1);
    _cManager->addConstraint(new RotationConstraint(4, Y, c1->getPoint(4)+Vector3(radius,0,0),r2,c1,true));

    Cloth* c2 = new Cloth(Vector2(clothWidth,clothLength), -triSize, Vector3(radius,yOffset,-centerOffset), Y, _manager);
    _manager->addVerlet(c2);
    //Note: index 80 by averaging corner 0 (80) and 2 (160)
    _cManager->addConstraint(new RotationConstraint(4, Y, c2->getPoint(4)+Vector3(-radius,0,0),r2,c2,true));
    Cloth* c3 = new Cloth(Vector2(clothLength,clothWidth), triSize, Vector3(centerOffset-triSize/2.0,yOffset,-radius), Y, _manager);
    _manager->addVerlet(c3);
    _cManager->addConstraint(new RotationConstraint(80, Y, c3->getPoint(80)+Vector3(0,0,radius),r2,c3,true));
    Cloth* c4 = new Cloth(Vector2(clothLength,clothWidth), -triSize, Vector3(-centerOffset+triSize/2.0,yOffset,radius), Y, _manager);
    _manager->addVerlet(c4);
    _cManager->addConstraint(new RotationConstraint(80, Y, c4->getPoint(80)+Vector3(0,0,-radius),r2,c4,true));

    //Constraints for connecting cloth, so they remain spaced evenly as they rotate
    _cManager->addConstraint(new VerletConstraint(4,c1,80,c3));
    _cManager->addConstraint(new VerletConstraint(4,c1,80,c4));
    _cManager->addConstraint(new VerletConstraint(4,c2,80,c3));
    _cManager->addConstraint(new VerletConstraint(4,c2,80,c4));
    //Shear
    _cManager->addConstraint(new VerletConstraint(4,c1,4,c2));
    _cManager->addConstraint(new VerletConstraint(80,c4,80,c3));
}

VerletTest2::~VerletTest2(){}

void VerletTest2::onTick(float seconds){
    //dragging
    if(dragMode&&_manager->solve){
        draggedMouse = _ray->getPointonPlane(draggedVerlet->getPoint(draggedPoint),-1*_camera->getLook());
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
