#include "token.h"
#include "engine/common/ellipsoid.h"
#include "engine/common/entity.h"
#include "engine/common/graphic.h"

Token::Token(const Vector3 &pos, Entity* t): Collectible(),
    size(.2)
{
    shape = new Ellipsoid(pos,Vector3(size,size,size));
    range = 3;
    target = t;
}

Token::~Token()
{
    delete shape;
}

void Token::onTick(float seconds)
{
    //Follows player after being collected
    if(collect){
        Vector3 currentPos = shape->getPos();
        Vector3 interpolate = Vector3::lerp(currentPos, target->getPos(), 1 - powf(0.08, seconds));
        shape->setPos(interpolate);
    }
    //Removed once it's close to the player
    Vector3 distance = shape->getPos()-target->getPos();
    if(distance.lengthSquared()<.5)
        remove = true;
}

void Token::onDraw(Graphic* g){
    //Changes color when it's collected
    if(collect)
        g->setColor(Vector3(1,0,0));
    else
        g->setColor(Vector3(1,1,1));
    Collectible::onDraw(g);
}

void Token::onCollect(Entity* )
{
      collect = true;
}

