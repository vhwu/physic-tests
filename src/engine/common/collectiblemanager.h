#ifndef COLLECTIBLEMANAGER_H
#define COLLECTIBLEMANAGER_H

#include "engine/common/manager.h"
#include <vector>

class Entity;
class Collectible;

//Manages collectibles.
//Triggers onCollect for each (if !toCollect) when Entity is in range, and
//removes them (if onRemove)
class CollectibleManager: public Manager
{
public:
    CollectibleManager(Entity* e);
    ~CollectibleManager();

    void onTick(float seconds);
    void onDraw(Graphic *g);

    void addCollectible(Collectible* c);
private:
    //Collectible objects manager maintains
    std::vector<Collectible*> collectibles;

    //The entity which collects the collectibles
    Entity* focal;
};

#endif // COLLECTIBLEMANAGER_H
