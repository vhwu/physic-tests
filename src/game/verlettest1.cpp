#include "verlettest1.h"
#include "engine/common/camera.h"
#include "player.h"
#include "engine/common/graphic.h"
#include "engine/verlet/verletmanager.h"
#include "engine/verlet/cloth.h"

VerletTest1::VerletTest1(Screen *s): World(s)
{
    _height = 3;
    _camera = new Camera(Vector2(800, 600), false);

    _player = new Player(_camera,_height);
    _player->setPos(Vector3(-3,3,-3));
    this->addEntity(_player);

    _manager = new VerletManager();
    this->addManager(_manager);

    //VERLETS
    Cloth* c1 = new Cloth(Vector2(20,20), .3, Vector3(0,0,0), 2, _manager);
    c1->pinCorner(0);
    c1->pinCorner(1);
    c1->pinCorner(2);
    c1->pinCorner(3);

    _manager->addVerlet(c1);

//    Cloth* c2 = new Cloth(Vector2(10,10), .3, Vector3(-2,0,-2), 1, _manager);
//    Cloth* c3 = new Cloth(Vector2(10,10), .3, Vector3(-2,0,-2), 3, _manager);
//    _manager->addVerlet(c2);
//    _manager->addVerlet(c3);

    //Raytracing while toggling between stationary + moveable mouse
    if(mouseMove)
        _ray = new RayTracer(_camera->modelview, mouseX, mouseY);
    else
        _ray = new RayTracer(_camera->getPos(), _camera->getLook());
}

VerletTest1::~VerletTest1()
{
    delete _camera;
    delete _player;
    delete _manager;
}
void VerletTest1::onTick(float seconds){
    //verlet collisions: offset player if not colliding with ground
    _manager->collideTerrain(_player);
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
    _manager->rayTrace(_ray, trace);
    hit = trace;

    //dragging
    if(dragMode){
        draggedMouse = _ray->getPointonPlane(draggedVerlet->getPoint(draggedPoint),-1*_camera->getLook());
        interpolate = Vector3::lerp(interpolate, draggedMouse, 1 - powf(0.01, seconds));
        draggedVerlet->setPos(draggedPoint,interpolate);
    }
    //update verlet
    World::onTick(seconds);
}

void VerletTest1::onDraw(Graphic *g){
    World::onDraw(g);

    //VISUALIZATION
    if(dragMode){
        //draw selected point in red
        g->setColor(Vector3(1,0,0));
        g->transform(&Graphic::drawUnitSphere,draggedVerlet->getPoint(draggedPoint),0,
            Vector3(.1,.1,.1));
        //draw mouse pos in cyan
        g->setColor(Vector3(0,1,1));
        g->transform(&Graphic::drawUnitSphere,draggedMouse,0,
            Vector3(.1,.1,.1));
        //draw interpolated position in green
        g->setColor(Vector3(0,1,0));
        g->transform(&Graphic::drawUnitSphere,interpolate,0,
                     Vector3(.1,.1,.1));
    }
    else if(hit.hit){
        //draw point that's being hovered in yellow
        g->setColor(Vector3(1,1,0));
        g->transform(&Graphic::drawUnitSphere,hit.v->getPoint(hit.id),0,
            Vector3(.1,.1,.1));
    }
}

void VerletTest1::mousePressEvent(QMouseEvent *event){
    World::mousePressEvent(event);
    if(event->button() == Qt::LeftButton&& hit.hit){
        dragMode = true;
        draggedPoint = hit.id;
        draggedVerlet = hit.v;
        interpolate = hit.v->getPoint(hit.id);
    }
    if(event->type() == QEvent::MouseButtonDblClick && event->button() == Qt::LeftButton)
        _manager->enableSolve();
}

void VerletTest1::mouseMoveEvent(QMouseEvent *event){
    World::mouseMoveEvent(event);
}

void VerletTest1::mouseReleaseEvent(QMouseEvent *event){
    World::mouseReleaseEvent(event);
    if(event->button() == Qt::LeftButton)
        dragMode = false;
}

void VerletTest1::wheelEvent(QWheelEvent *event){
    _camera->zoom(event->delta());
}

void VerletTest1::keyPressEvent(QKeyEvent *event){
    World::keyPressEvent(event);
    _player->keyPressEvent(event);
}

void VerletTest1::keyReleaseEvent(QKeyEvent *event){
     _player->keyReleaseEvent(event);
}

