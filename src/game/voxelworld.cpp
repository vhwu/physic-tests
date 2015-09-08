#include "voxelworld.h"
#include "engine/common/camera.h"
#include "player.h"
#include "engine/common/graphic.h"
#include "engine/voxel/voxelmanager.h"
#include "engine/voxel/voxelray.h"
#include "engine/common/valuenoise.h"
#include "constants.h"
#include "engine/common/frustum.h"

#include <iostream>

VoxelWorld::VoxelWorld(): World(), ChunkGenerator()
{
    _height = 2;
    _camera = new Camera(Vector2(800, 600), false);
    _camera->translate(Vector3(0,_height,0));

    _player = new Player(_camera,_height);
    this->addEntity(_player);
    _player->move(Vector3(0,36,0));

    _noise = new ValueNoise();
    _manager = new VoxelManager(_camera->getFrustum(), _player, this);
    _ray = new VoxelRay(_camera, _manager);

    _manager->initialize();
}

VoxelWorld::~VoxelWorld()
{
    delete _camera;
    delete _player;
    delete _manager;
    delete _noise;
    delete _ray;
}

void VoxelWorld::generateChunk(const Vector3& v){
    int _dim = constants::_dim;
    Vector3 start = v*_dim;
    Vector3 end = start+_dim;
    for(int i=start.z;i<end.z;i++){
        for(int j=start.x;j<end.x;j++){
            int val =(int) (_noise->valueNoise(Vector2(abs(j),abs(i)))*50);// + 25;
            for(int k=0;k<val;k++){
                if(k<15&&k>start.y&&k<end.y)
                    _manager->addBlock('g', Vector3(j,k,i));
                else if(k>start.y&&k<end.y)
                    _manager->addBlock('s', Vector3(j,k,i));
            }
        }
    }
}


void VoxelWorld::onTick(float seconds){
    World::onTick(seconds);
    _manager->onTick(seconds);
    _ray->onTick(seconds);

    //Collide entities against voxels
    foreach (Entity* e, _entities)
        e->setMtv(_manager->collide(e));

    //Reset vbo after addition/ removal of blocks:
    if(update){
        _manager->updateChunk(updateVbo);
        update = false;
    }
}

void VoxelWorld::onDraw(Graphic *g){
    World::onDraw(g);
    _manager->onDraw(g);

    //Raycasting
    if(_ray->_intersect){
        g->setColor(Vector3(1,0,0));
        g->drawFace(_ray->_hit, _ray->_face);
        //g->transform(&Graphic::drawUnitCube, *hit-Vector3(.1,.1,.1),
        //             0, Vector3(1.2,1.2,1.2));
    }
}

void VoxelWorld::removeBlock(){
    if(_ray->_intersect){
        _manager->removeBlock(_ray->_hit);
        update = true;
        updateVbo = _ray->_hit;
    }
}

void VoxelWorld::addBlock(){
    if(_ray->_intersect){
        _manager->addBlock('g', _ray->_prev);
        update = true;
        updateVbo = _ray->_prev;
    }
}

void VoxelWorld::drawBG(Graphic *g){}

void VoxelWorld::mousePressEvent(QMouseEvent *event){
}
void VoxelWorld::rotateCamera(const Vector2& delta){
    World::rotateCamera(delta);
}
void VoxelWorld::mouseMoveEvent(QMouseEvent *event){
}
void VoxelWorld::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::RightButton)
        removeBlock();
    if(event->button() == Qt::LeftButton)
        addBlock();

}
void VoxelWorld::wheelEvent(QWheelEvent *event){
    _camera->zoom(event->delta());
}
void VoxelWorld::keyPressEvent(QKeyEvent *event){
    _player->keyPressEvent(event);
}
void VoxelWorld::keyReleaseEvent(QKeyEvent *event){
    _player->keyReleaseEvent(event);
}
