#ifndef VOXELWORLD_H
#define VOXELWORLD_H

#include "engine/common/world.h"
#include "vector.h"
#include "engine/voxel/voxelray.h"
#include "engine/voxel/chunkgenerator.h"

class ValueNoise;
class Player;
class VoxelManager;

class VoxelWorld: public World, public ChunkGenerator
{
public:
    VoxelWorld();
    ~VoxelWorld();

    //Collides entities against voxels, updates terrain based on player input
    void onTick(float seconds);
    //Draws player, terrain, and intersected face from raycast
    void onDraw(Graphic *g);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    //i to add highlighted block, o to remove highlighted block
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void rotateCamera(const Vector2& delta);

    //*********load
    void generateChunk(const Vector3& v);
protected:
    void drawBG(Graphic *g);
private:
    Player* _player;
    float _height;
    VoxelManager* _manager;
    ValueNoise* _noise;
    VoxelRay* _ray;

    //For player addition/ removal of blocks
    //Set to true if terrain was edited this tick
    bool update;
    //Abs pos of block- used to find chunk to update
    Vector3 updateVbo;

    void removeBlock();
    void addBlock();


};

#endif // VOXELWORLD_H
