#include "verlettest4.h"
#include "engine/verlet/rotationconstraint.h"
#include "engine/verlet/setupcircle.h"

VerletTest4::VerletTest4(Screen *s): VerletLevel(s){
    //GRAVITY
    _manager->gravity = Vector3(0,-2,0);
    this->_gravity = Vector3(0,0,0);

    //Verlet variables
    int numVerlets = 11;
    int numTriangles = 1; //on edge of verlet along circle
    Vector2 dim = Vector2(numTriangles+1,20);
    float percentVerlet = .3; //ratio between verlet:space on circle

    //Circle variables
    Axis a = Y;
    float radius = 2;
    Vector3 center = Vector3(0,0,0);

    //Setup circle
    circle = new SetupCircle(radius,center,a);
    circle->setAngle(30);

    //Verlets
    SetupInfo test = circle->positionVerlets(numVerlets,percentVerlet,numTriangles);
    for(int i = 0; i<numVerlets; i++){
        Cloth* v = new Cloth(dim, test.triSize, test.startPos[i], a, _manager, test.angles[i]);
        _manager->addVerlet(v);
        _cManager->addConstraint(new RotationConstraint(v->getCorner(0),a,center,radius,v));
        _cManager->addConstraint(new RotationConstraint(v->getCorner(1),a,center,radius,v));
    }
}

VerletTest4::~VerletTest4()
{}

void VerletTest4::onTick(float seconds){
    //dragging
    if(dragMode&&_manager->solve){
        draggedMouse = _ray->getPointonPlane(draggedVerlet->getPoint(draggedPoint),-1*_camera->getLook());
        interpolate = Vector3::lerp(interpolate, draggedMouse, 1 - powf(_mouseSpeed, seconds));
        draggedVerlet->setPos(draggedPoint,interpolate);
    }
    VerletLevel::onTick(seconds);
}

void VerletTest4::onDraw(Graphic *g){
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

    //Visualize axes: XYZ = RGB, w/ sphere at 'positive' end
    Vector3 center = Vector3(0,0,0);
    float radius = 3;
    for(int i = 0; i<3; i++){
        g->setColor(axis_color[i]);
        g->drawCircle((Axis)i,radius,center);
        Vector3 point = Vector3(0,0,0);
        point.xyz[i]=radius;
        g->transform(&Graphic::drawUnitSphere,point,0,
                     Vector3(.1,.1,.1));
    }

    //Visualize setup
    circle->onDraw(g);
    VerletLevel::onDraw(g);
}

//Set if constraint is manipulated
void VerletTest4::mousePressEvent(QMouseEvent *event){
    VerletLevel::mousePressEvent(event);

    if(event->button() == Qt::LeftButton&& hit.hit){
        _cManager->setSelection(hit.c);
        dragMode = true;
        draggedPoint = hit.id;
        draggedVerlet = hit.v;
        interpolate = hit.v->getPoint(hit.id);
    }
}

void VerletTest4::mouseMoveEvent(QMouseEvent *event){
    VerletLevel::mouseMoveEvent(event);
}

//Set if constraint is deselected
void VerletTest4::mouseReleaseEvent(QMouseEvent *event){
    VerletLevel::mouseReleaseEvent(event);
    if(event->button() == Qt::LeftButton){
        dragMode = false;
        _cManager->resetSelection();
    }
}
