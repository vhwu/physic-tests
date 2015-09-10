#include "verlettest1.h"
#include "engine/common/camera.h"
#include "player.h"
#include "engine/common/graphic.h"
#include "engine/verlet/verletmanager.h"
#include "engine/verlet/cloth.h"
#include "engine/verlet/verlet.h"
#include "token.h"
#include "engine/common/collectiblemanager.h"

VerletTest1::VerletTest1(Screen *s): World(s)
{
    _height = 3;
    _camera = new Camera(Vector2(800, 600), false);

    _player = new Player(_camera,_height);
    _player->setPos(Vector3(-1,3,-1));
    this->addEntity(_player);

    _manager = new VerletManager();
    this->addManager(_manager);

    //VERLETS
    Cloth* start = new Cloth(Vector2(12,12), .3, Vector3(0,0,0), Y, _manager);
    start->pinCorners();
    Cloth* end = new Cloth(Vector2(12,12), .3, Vector3(-26,4,0), Y, _manager);
    end->pinCorners();
    _manager->addVerlet(end);
    _manager->addVerlet(start);

    Cloth* c1 = new Cloth(Vector2(12,90), .3, Vector3(-3.5,0,0), Y, _manager);
    //Offset further two corners, to give cloth some slack
    std::cout<<c1->getCorner(2)<<std::endl;
    Vector3 test1 = c1->getPoint(c1->getCorner(2));
    Vector3 test2 = c1->getPoint(c1->getCorner(3));
    c1->setPos(c1->getCorner(2),Vector3(-25,test1.y,test1.z));
    c1->setPos(c1->getCorner(3),Vector3(-25,test2.y,test2.z));

    //Constrain corners to translation on y-axis
    c1->createTranslate(c1->getCorner(0),  Y, 5);
    c1->createTranslate(c1->getCorner(1),  Y, 5);
    c1->createTranslate(c1->getCorner(2),  Y, 5);
    c1->createTranslate(c1->getCorner(3),  Y, 5);

    //Create player-controlled constraint
    c1->createTranslate(6,  Y, 5);
    c1->addSelect(6);

    _manager->addVerlet(c1);

    //COLLECTIBLES
    CollectibleManager* cm = new CollectibleManager(_player);
    this->addManager(cm);

    Token* token0 = new Token(Vector3(-4,-1,-1.5),_player);
    Token* token1 = new Token(Vector3(-7,-2,-1.5),_player);
    Token* token2 = new Token(Vector3(-10,-1,-1.5),_player);
    Token* token3 = new Token(Vector3(-13,0,-1.5),_player);
    Token* token4 = new Token(Vector3(-16,1,-1.5),_player);
    Token* token5 = new Token(Vector3(-19,2,-1.5),_player);
    Token* token6 = new Token(Vector3(-22,1,-1.5),_player);
    Token* token10 = new Token(Vector3(-2,-1.2,-1.3),_player);
    Token* token11 = new Token(Vector3(-4,-0.3,-1.6),_player);
    Token* token12 = new Token(Vector3(-11,.3,-1.4),_player);
    Token* token13 = new Token(Vector3(-14,0.1,-1.8),_player);
    Token* token14 = new Token(Vector3(-16,-.2,-1.2),_player);
    Token* token15 = new Token(Vector3(-18,.14,-1.25),_player);
    Token* token16 = new Token(Vector3(-21,1,-1.7),_player);

    cm->addCollectible(token0);
    cm->addCollectible(token1);
    cm->addCollectible(token2);
    cm->addCollectible(token3);
    cm->addCollectible(token4);
    cm->addCollectible(token5);
    cm->addCollectible(token6);
    cm->addCollectible(token10);
    cm->addCollectible(token11);
    cm->addCollectible(token12);
    cm->addCollectible(token13);
    cm->addCollectible(token14);
    cm->addCollectible(token15);
    cm->addCollectible(token16);

    //Raytracing while toggling between stationary + moveable mouse
    if(mouseMove)
        _ray = new RayTracer(_camera->modelview, mouseX, mouseY);
    else
        _ray = new RayTracer(_camera->getPos(), _camera->getLook());
}

VerletTest1::~VerletTest1()
{
    delete _ray;
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
        //draw point that's being hovered in green if it's selectable, yellow if not
        if(hit.v->canSelect(hit.id))
            g->setColor(Vector3(0,1,0));
        else
            g->setColor(Vector3(1,1,0));
        g->transform(&Graphic::drawUnitSphere,hit.v->getPoint(hit.id),0,
            Vector3(.1,.1,.1));
    }
    World::onDraw(g);
}

void VerletTest1::mousePressEvent(QMouseEvent *event){
    World::mousePressEvent(event);
    if(event->button() == Qt::LeftButton&& hit.hit && hit.v->canSelect(hit.id)){
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
    if(event->key() == Qt::Key_F)
        _manager->enableSolve();
    _player->keyPressEvent(event);
}

void VerletTest1::keyReleaseEvent(QKeyEvent *event){
     _player->keyReleaseEvent(event);
}

