#include "verlettest1.h"
#include "engine/verlet/translationconstraint.h"
#include "token.h"
#include "engine/common/collectiblemanager.h"

VerletTest1::VerletTest1(Screen *s): VerletLevel(s){
    //Player starting position
    _startPos = Vector3(-1,3,1.5);
    _player->setPos(_startPos);

    //MANAGERS
    CollectibleManager* cm = new CollectibleManager(this,_player);
    this->addManager(cm);

    //VERLETS
    int angle = 270;
    float size = .3;
    Cloth* start = new Cloth(Vector2(12,12), size, Vector3(0,0,0), Y, _vManager, angle);
    start->pinCorners();
    Cloth* end = new Cloth(Vector2(12,12), size, Vector3(-26,4,0), Y, _vManager, angle);
    end->pinCorners();
    _vManager->addVerlet(end);
    _vManager->addVerlet(start);

    Cloth* c1 = new Cloth(Vector2(12,90), size, Vector3(-3.5,0,0), Y, _vManager, angle);
    //Offset further two corners, to give cloth some slack
    Vector3 test1 = c1->getPoint(c1->getCorner(2));
    Vector3 test2 = c1->getPoint(c1->getCorner(3));
    c1->setPos(c1->getCorner(2),Vector3(-25,test1.y,test1.z));
    c1->setPos(c1->getCorner(3),Vector3(-25,test2.y,test2.z));
    _vManager->addVerlet(c1);

    //CONSTRAINTS
    //Constrain corners to translation on y-axis
    _cManager->addConstraint(new TranslationConstraint(c1->getCorner(0), Y, 5, c1));
    _cManager->addConstraint(new TranslationConstraint(c1->getCorner(1), Y, 5, c1));
    _cManager->addConstraint(new TranslationConstraint(c1->getCorner(2), Y, 5, c1));
    _cManager->addConstraint(new TranslationConstraint(c1->getCorner(3), Y, 5, c1));
    //Create player-controlled constraint
    _cManager->addConstraint(new TranslationConstraint(6, Y, 7, c1,true));

    //COLLECTIBLES
    float offsetZ = 1.5;
    cm->addCollectible(new Token(Vector3(-4,-1,offsetZ),_player));
    cm->addCollectible(new Token(Vector3(-7,-2,offsetZ),_player));
    cm->addCollectible(new Token(Vector3(-10,-1,offsetZ),_player));
    cm->addCollectible(new Token(Vector3(-13,0,offsetZ),_player));
    cm->addCollectible(new Token(Vector3(-16,1,offsetZ),_player));
    cm->addCollectible(new Token(Vector3(-19,2,offsetZ),_player));
    cm->addCollectible(new Token(Vector3(-22,1,offsetZ),_player));
    //Randomly distributed
    cm->addCollectible(new Token(Vector3(-2,-1.2,offsetZ),_player));
    cm->addCollectible(new Token(Vector3(-4,-0.3,offsetZ),_player));
    cm->addCollectible(new Token(Vector3(-11,.3,offsetZ),_player));
    cm->addCollectible(new Token(Vector3(-14,0.1,offsetZ),_player));
    cm->addCollectible(new Token(Vector3(-16,-.2,offsetZ),_player));
    cm->addCollectible(new Token(Vector3(-18,.14,offsetZ),_player));
    cm->addCollectible(new Token(Vector3(-21,1,offsetZ),_player));
}

VerletTest1::~VerletTest1()
{
}
