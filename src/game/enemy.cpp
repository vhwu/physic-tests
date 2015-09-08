#include "enemy.h"
#include "engine/common/graphic.h"
#include "engine/common/ellipsoid.h"
#include "engine/geometric/geometricmanager.h"
#include "engine/geometric/graph.h"
#include "player.h"

Enemy::Enemy(const Vector3& pos, Player* p, GeometricManager* m): Entity()
{
    _shape = new Ellipsoid(pos, Vector3(.5,1,.5));
    _player = p;
    _manager = m;
}

Enemy::~Enemy()
{
    delete _shape;
}

Ellipsoid* Enemy::getEllipsoid(){
    return (Ellipsoid*) _shape;
}

void Enemy::onTick(float seconds)
{
    //raytrace information from player
    HitTest target = _player->rayDown;
    //if both are on the mesh, find the path between them
    if(rayDown.hit&&target.hit){
        _path = _manager->_graph->returnPath(
                    this->getPos(),rayDown.tri,
                    _player->getPos(),target.tri);
    }
    //if there are corners in the path, offset the next point
    Vector3 direction = Vector3(0,0,0);
    if(_path.size()>2){
        Vector3 v1 = _path[0]-_path[1];
        Vector3 v2 = _path[2]-_path[1];
        v1.y=0;
        v2.y=0;
        v1.normalize();
        v2.normalize();
        Vector3 average = v1+v2;
        average*=-1; //invert
        average.normalize();
        Vector3 next = _path[1]+average*3;
        nextPoint = _path[1]+average;
        direction = next-_path[0];
    }
    else if(_path.size()>1){
        direction = _path[1]-_path[0];
    }
    _vel = direction*.6;
    Entity::onTick(seconds);
}

void Enemy::move(const Vector3& translate){
    _shape->translate(translate);
}

void Enemy::onDraw(Graphic* g){
    g->setColor(Vector3(1,0,0));
    Entity::onDraw(g);
    //draw funnel
    /*
    if(_path.size()>0){
        for (int i = 0; i<_path.size()-1; i++){
            Vector3 p1 = _path[i];
            Vector3 p2 = _path[i+1];
            g->drawLine(p1,p2);
        }
    }
    */
}
