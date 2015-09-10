#include "collectible.h"
#include "engine/common/graphic.h"
#include "shape.h"


Collectible::Collectible():
      collect(false),
      remove(false)
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
    collect = true;
    remove = true;
}

void Collectible::onDraw(Graphic* g){
    shape->onDraw(g);
}

