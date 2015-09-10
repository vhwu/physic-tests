#ifndef TOKEN_H
#define TOKEN_H

#include "engine/common/collectible.h"

class Entity;

//Collectible that changes color and is drawn to target Entity after being collected
class Token: public Collectible
{
public:
    Token(const Vector3& pos, Entity *t);
    ~Token();

    void onTick(float seconds);
    void onDraw(Graphic* g);
    void onCollect(Entity* e);
private:
    float size;
    Entity* target;
};

#endif // TOKEN_H
