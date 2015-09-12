#include "verlettest1.h"
#include "engine/verlet/translationconstraint.h"
#include "token.h"
#include "engine/common/collectiblemanager.h"

VerletTest1::VerletTest1(Screen *s): VerletLevel(s){
    //MANAGERSs
    CollectibleManager* cm = new CollectibleManager(_player);
    this->addManager(cm);

    //VERLETS
    Cloth* start = new Cloth(Vector2(12,12), .3, Vector3(0,0,0), Y, _manager);
    start->pinCorners();
    Cloth* end = new Cloth(Vector2(12,12), .3, Vector3(-26,4,0), Y, _manager);
    end->pinCorners();
    _manager->addVerlet(end);
    _manager->addVerlet(start);

    Cloth* c1 = new Cloth(Vector2(12,90), .3, Vector3(-3.5,0,0), Y, _manager);
    //Offset further two corners, to give cloth some slack
    Vector3 test1 = c1->getPoint(c1->getCorner(2));
    Vector3 test2 = c1->getPoint(c1->getCorner(3));
    c1->setPos(c1->getCorner(2),Vector3(-25,test1.y,test1.z));
    c1->setPos(c1->getCorner(3),Vector3(-25,test2.y,test2.z));
    _manager->addVerlet(c1);

    //CONSTRAINTS
    //Constrain corners to translation on y-axis
    _cManager->addConstraint(new TranslationConstraint(c1->getCorner(0), Y, 5, c1));
    _cManager->addConstraint(new TranslationConstraint(c1->getCorner(1), Y, 5, c1));
    _cManager->addConstraint(new TranslationConstraint(c1->getCorner(2), Y, 5, c1));
    _cManager->addConstraint(new TranslationConstraint(c1->getCorner(3), Y, 5, c1));
    //Create player-controlled constraint
    _cManager->addConstraint(new TranslationConstraint(6, Y, 7, c1,true));

    //COLLECTIBLES
    cm->addCollectible(new Token(Vector3(-4,-1,-1.5),_player));
    cm->addCollectible(new Token(Vector3(-7,-2,-1.5),_player));
    cm->addCollectible(new Token(Vector3(-10,-1,-1.5),_player));
    cm->addCollectible(new Token(Vector3(-13,0,-1.5),_player));
    cm->addCollectible(new Token(Vector3(-16,1,-1.5),_player));
    cm->addCollectible(new Token(Vector3(-19,2,-1.5),_player));
    cm->addCollectible(new Token(Vector3(-22,1,-1.5),_player));
    //Randomly distributed
    cm->addCollectible(new Token(Vector3(-2,-1.2,-1.3),_player));
    cm->addCollectible(new Token(Vector3(-4,-0.3,-1.6),_player));
    cm->addCollectible(new Token(Vector3(-11,.3,-1.4),_player));
    cm->addCollectible(new Token(Vector3(-14,0.1,-1.8),_player));
    cm->addCollectible(new Token(Vector3(-16,-.2,-1.2),_player));
    cm->addCollectible(new Token(Vector3(-18,.14,-1.25),_player));
    cm->addCollectible(new Token(Vector3(-21,1,-1.7),_player));
}

VerletTest1::~VerletTest1()
{
}

void VerletTest1::onTick(float seconds){
    //dragging
    if(dragMode&&_manager->solve){
        draggedMouse = _ray->getPointonPlane(draggedVerlet->getPoint(draggedPoint),-1*_camera->getLook());
        interpolate = Vector3::lerp(interpolate, draggedMouse, 1 - powf(_mouseSpeed, seconds));
        draggedVerlet->setPos(draggedPoint,interpolate);
    }
    VerletLevel::onTick(seconds);
}

void VerletTest1::onDraw(Graphic *g){
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
void VerletTest1::mousePressEvent(QMouseEvent *event){
    VerletLevel::mousePressEvent(event);

    if(event->button() == Qt::LeftButton&& hit.hit){
        _cManager->setSelection(hit.c);
        dragMode = true;
        draggedPoint = hit.id;
        draggedVerlet = hit.v;
        interpolate = hit.v->getPoint(hit.id);
    }
}

void VerletTest1::mouseMoveEvent(QMouseEvent *event){
    VerletLevel::mouseMoveEvent(event);
}

//Set if constraint is deselected
void VerletTest1::mouseReleaseEvent(QMouseEvent *event){
    VerletLevel::mouseReleaseEvent(event);
    if(event->button() == Qt::LeftButton){
        dragMode = false;
        _cManager->resetSelection();
    }
}
