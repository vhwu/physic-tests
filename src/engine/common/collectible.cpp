#include "collectible.h"
#include "engine/common/graphic.h"
#include "shape.h"


Collectible::Collectible():
      toCollect(false),
      toRemove(false)
{
}

Collectible::~Collectible()
{
    delete shape;
}

void Collectible::onTick(float )
{
}

void Collectible::onCollect(Entity* )
{
    toCollect = true;
    toRemove = true;
}

void Collectible::onDraw(Graphic* g){
    shape->onDraw(g);
}

