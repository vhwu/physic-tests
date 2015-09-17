#include "verletlevel.h"
#include "engine/verlet/constraint.h"


VerletLevel::VerletLevel(Screen *s): World(s),
    _height(3),
    _startPos(Vector3(-1,3,-1)),
    _mouseSpeed(0.12)
{
    _player = new Player(_camera,_height);
    _player->setPos(_startPos);
    this->addEntity(_player);

    //MANAGERS
    _cManager = new ConstraintManager(this);
    this->addManager(_cManager);
    _vManager = new VerletManager(this,_cManager);
    this->addManager(_vManager);
}

VerletLevel::~VerletLevel()
{}

void VerletLevel::onTick(float seconds){
    //Verlet collisions: offset player if not colliding with ground
    _player->setMtv(_vManager->collideTerrain(_player));
    _player->move(_player->getMove());

    //Selection
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

void VerletLevel::onDraw(Graphic *g){
    World::onDraw(g);

    //Visualize mouse selection + interpolation
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
}

//Right click for freezing/ unfreezing
//Hover/ select
void VerletLevel::mousePressEvent(QMouseEvent *event){
    World::mousePressEvent(event);
    //if(event->type() == QEvent::MouseButtonDblClick && event->button() == Qt::LeftButton)
    if(event->button()==Qt::RightButton)
        _vManager->enableSolve();

    if(event->button() == Qt::LeftButton&& hit.hit){
        selectedC = hit.c;
        selectedC->setSelect(true);
        dragMode = true;
        interpolate = hit.v->getPoint(hit.id);
    }
}

//Set if constraint is hovered
void VerletLevel::mouseMoveEvent(QMouseEvent *event){
    World::mouseMoveEvent(event);
    if(hit.hit){
        if((hoveredC!=NULL)&&(hoveredC!=hit.c)){
            hoveredC->setHover(false);
        }
        hoveredC = hit.c;
        hoveredC->setHover(true);
    }
    else if(hoveredC!=NULL){
        hoveredC->setHover(false);
        hoveredC = NULL;
    }
}

//Reset constraint selection
void VerletLevel::mouseReleaseEvent(QMouseEvent *event){
    World::mouseReleaseEvent(event);

    if(event->button() == Qt::LeftButton){
        if(dragMode){
            selectedC->setSelect(false);
            selectedC = NULL;
        }
        dragMode = false;
    }
}

//Camera zoom
void VerletLevel::wheelEvent(QWheelEvent *event){
    _camera->zoom(event->delta());
}

//F:freeze
//U:reset player position
void VerletLevel::keyPressEvent(QKeyEvent *event){
    World::keyPressEvent(event);
    if(event->key() == Qt::Key_U)
        _player->resetPos(_startPos);
    _player->keyPressEvent(event);
}

void VerletLevel::keyReleaseEvent(QKeyEvent *event){
     _player->keyReleaseEvent(event);
}
