#include "verletlevel.h"

VerletLevel::VerletLevel(Screen *s): World(s),
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

    //Raytracing while toggling between stationary + moveable mouse
    if(mouseMove)
        _ray = new RayTracer(_camera->modelview, mouseX, mouseY);
    else
        _ray = new RayTracer(_camera->getPos(), _camera->getLook());
}

VerletLevel::~VerletLevel()
{
    delete _ray;
}

void VerletLevel::onTick(float seconds){
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

    World::onTick(seconds);
}

void VerletLevel::onDraw(Graphic *g){
    World::onDraw(g);
}

//Double click for freezing/ unfreezing
void VerletLevel::mousePressEvent(QMouseEvent *event){
    World::mousePressEvent(event);
    if(event->type() == QEvent::MouseButtonDblClick && event->button() == Qt::LeftButton)
        _manager->enableSolve();
}

//Set if constraint is hovered
void VerletLevel::mouseMoveEvent(QMouseEvent *event){
    World::mouseMoveEvent(event);
    if(hit.hit)
        _cManager->setHover(hit.c);
    else
        _cManager->resetHover();
}

void VerletLevel::mouseReleaseEvent(QMouseEvent *event){
    World::mouseReleaseEvent(event);
}

//Camera zoom
void VerletLevel::wheelEvent(QWheelEvent *event){
    _camera->zoom(event->delta());
}

//F:freeze
//U:reset player position
void VerletLevel::keyPressEvent(QKeyEvent *event){
    World::keyPressEvent(event);
    if(event->key() == Qt::Key_F)
        _manager->enableSolve();
    if(event->key() == Qt::Key_U)
        _player->resetPos(_startPos);
    _player->keyPressEvent(event);
}

void VerletLevel::keyReleaseEvent(QKeyEvent *event){
     _player->keyReleaseEvent(event);
}
