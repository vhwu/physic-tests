#include "collectiblemanager.h"
#include "entity.h"
#include "collectible.h"

CollectibleManager::CollectibleManager(Entity *e)
{
    focal = e;
}

CollectibleManager::~CollectibleManager()
{
    for (std::vector<Collectible*>::iterator it = collectibles.begin() ;
         it != collectibles.end(); ++it)
        delete (*it);
    collectibles.clear();
}

void CollectibleManager::onTick(float seconds){
    Vector3 focalPos = focal->getPos();

    for (unsigned i=0; i<collectibles.size(); ++i){
        Collectible* c = collectibles[i];
        c->onTick(seconds);

        //If it hasn't been collected, try to collect it
        if(!c->getCollect()){
            Vector3 distance = focalPos-c->getPos();
            if(distance.length()<c->range){
                c->onCollect(focal);
            }
        }

        if(c->getRemove())
            collectibles.erase(collectibles.begin()+i);
    }
}

void CollectibleManager::onDraw(Graphic *g){
    for (unsigned i=0; i<collectibles.size(); ++i){
        Collectible* c = collectibles[i];
        c->onDraw(g);
    }
}

void CollectibleManager::addCollectible(Collectible *c){
    collectibles.push_back(c);
}
