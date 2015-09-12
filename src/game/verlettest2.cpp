#include "verlettest2.h"
#include "engine/common/camera.h"
#include "player.h"
#include "engine/common/graphic.h"
#include "engine/verlet/verletmanager.h"
#include "engine/verlet/constraintmanager.h"
#include "engine/verlet/cloth.h"
#include "engine/verlet/verlet.h"
#include "engine/verlet/constraint.h"
#include "engine/verlet/rotationconstraint.h"
#include "engine/verlet/verletconstraint.h"


//Controls:
//U: move player back to starting positions
//F: freeze
VerletTest2::VerletTest2(Screen* s): World(s),
    _height(3),
    _startPos(Vector3(-1,3,-1)),
    _mouseSpeed(0.12)
{
    _camera = new Camera(Vector2(800, 600), false);

    _player = new Player(_camera,_height);
    _player->setPos(_startPos);
    this->addEntity(_player);

    //MANAGERS
    _cManager = new ConstraintManager();
    this->addManager(_cManager);
    _manager = new VerletManager(_cManager);
    this->addManager(_manager);
    //Allow for gravity
    _manager->gravity = Vector3(0,-1.5,0);

    //VERLETS
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

    _cManager->addConstraint(new VerletConstraint(4,c1,80,c3));
    _cManager->addConstraint(new VerletConstraint(4,c1,80,c4));
    _cManager->addConstraint(new VerletConstraint(4,c2,80,c3));
    _cManager->addConstraint(new VerletConstraint(4,c2,80,c4));
    //Shear
    _cManager->addConstraint(new VerletConstraint(4,c1,4,c2));
    _cManager->addConstraint(new VerletConstraint(80,c4,80,c3));



    //Raytracing while toggling between stationary + moveable mouse
    if(mouseMove)
        _ray = new RayTracer(_camera->modelview, mouseX, mouseY);
    else
        _ray = new RayTracer(_camera->getPos(), _camera->getLook());
}

VerletTest2::~VerletTest2()
{
    delete _ray;
}

void VerletTest2::onTick(float seconds){
    //verlet collisions: offset player if not colliding with ground
    _player->setMtv(_manager->collideTerrain(_player));
    _player->move(_player->getMove());

    //raytrace to find hovered point
    if(mouseMove){
        _ray->update(_camera->modelview, mouseX, mouseY);
    }
    else{
        _ray->source = _camera->getPos();
        _ray->direction = _camera->getLook();
    }

    HitTest trace;
    _cManager->rayTrace(_ray,trace);
    hit = trace;

    //dragging
    if(dragMode&&_manager->solve){
        draggedMouse = _ray->getPointonPlane(draggedVerlet->getPoint(draggedPoint),-1*_camera->getLook());
        interpolate = Vector3::lerp(interpolate, draggedMouse, 1 - powf(_mouseSpeed, seconds));
        draggedVerlet->setPos(draggedPoint,interpolate);
    }

    World::onTick(seconds);
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
    World::onDraw(g);
}

//Set if constraint is manipulated
void VerletTest2::mousePressEvent(QMouseEvent *event){
    World::mousePressEvent(event);
    if(event->button() == Qt::LeftButton&& hit.hit){
        _cManager->setSelection(hit.c);
        dragMode = true;
        draggedPoint = hit.id;
        draggedVerlet = hit.v;
        interpolate = hit.v->getPoint(hit.id);
    }
    if(event->type() == QEvent::MouseButtonDblClick && event->button() == Qt::LeftButton)
        _manager->enableSolve();
}

//Set if constraint is hovered
void VerletTest2::mouseMoveEvent(QMouseEvent *event){
    World::mouseMoveEvent(event);
    if(hit.hit)
        _cManager->setHover(hit.c);
    else
        _cManager->resetHover();
}

//Set if constraint is deselected
void VerletTest2::mouseReleaseEvent(QMouseEvent *event){
    World::mouseReleaseEvent(event);
    if(event->button() == Qt::LeftButton){
        dragMode = false;
        _cManager->resetSelection();
    }
}

//Camera zoom
void VerletTest2::wheelEvent(QWheelEvent *event){
    _camera->zoom(event->delta());
}

//F:freeze
//U:reset player position
void VerletTest2::keyPressEvent(QKeyEvent *event){
    World::keyPressEvent(event);
    if(event->key() == Qt::Key_F)
        _manager->enableSolve();
    if(event->key() == Qt::Key_U)
        _player->resetPos(_startPos);
    _player->keyPressEvent(event);
}

void VerletTest2::keyReleaseEvent(QKeyEvent *event){
     _player->keyReleaseEvent(event);
}
