#ifndef CHUNK_H
#define CHUNK_H

#include "vector.h"
#include "vbo.h"
#include "voxelmanager.h"

class VBO;
struct Block;
class Graphic;


class Chunk
{
public:
    Chunk(const Vector3& pos, VoxelManager* m);
    virtual ~Chunk();
    Vector3 getPos(){return _pos;}

    //Loops:
    void onTick(float seconds);
    //Cycle through and draw blocks
    void onDraw(Graphic* g);

    //Generation (called by corresponding methods in VoxelManager):
    //Use relative coordinates
    void addBlock(const Vector3& pos, char type);
    void removeBlock(const Vector3& pos);

    //For storing information in vbo:
    //Call ONCE after generation + world's initGL
    void initVbo();
    //Call when chunk changes, such as when block is added/ removed by player
    void setFaceTransp();
    void resetVbo();
    VBO* _vbo;

    //Points to test for frustum culling
    Vector3 _corners[8];

    //helper for setFaceTransp, to check opacity of neighboring blocks (relative index)
    bool checkTransp(const Vector3& index);
    //helper for collideShape (manager), to check passability of blocks (relative index)
    bool checkPass(const Vector3& index);

    //default terrain generation: fill chunk if it's at/ below a y of 0
    void fill();
private:
    VoxelManager* _manager;
    Vector3 _pos; //pos in grid space, not absolute space

    static const int _dim = constants::_dim; //blocks in chunk
    Block* _blocks[_dim*_dim*_dim]={}; //widthxheightxdepth

    //Not used, kept for record: combines resetVbo + setFaceTransp
    //Doesn't seem much faster, less clear
    void setVbo();

};

#endif // CHUNK_H
