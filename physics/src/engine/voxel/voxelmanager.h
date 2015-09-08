#ifndef VOXELMANAGER_H
#define VOXELMANAGER_H

#include <QKeyEvent>
#include <deque>
#include "vector.h"
#include "constants.h"

class Graphic;
struct Block;
class Chunk;
class Frustum;
class Entity;
class ChunkGenerator;

using constants::v3_pointer;

//Loads + retrieves block types
//Chunk creation + update: utility for adding/ removing blocks, updating vbos
//Automatically loads/ unloads chunks
class VoxelManager
{
public:
    VoxelManager(Frustum* f, Entity* e, ChunkGenerator* g);
    ~VoxelManager();

    typedef struct{
        //6 vector2s representing sprites for each face
        Vector2* _textures;
        bool _pass;
        bool _trans;
    }blockType;

    blockType getType(Block* b);
    blockType getType(char c);

    //Loading
    void initialize();  //call for initial terrain generation

    //Called by _generator (world) to add/ edit terrain
    //Use absolute (33 55 77) coordinates
    void addBlock(char c, const Vector3& pos);
    bool checkBlock(const Vector3& nextPos);
    void removeBlock(const Vector3& pos);
    //Call if vbo of chunk (relative coord) needs update, onTick of world
    void updateChunk(const Vector3& chunkPos);

    //Pass calls from containing world to chunks
    void onTick(float seconds);
    //Draws all chunks within range, w/ frustum culling
    void onDraw(Graphic *g);
    //Collision of terrain with entities- returns 'mtv'
    Vector3 collide(Entity* e);

    int _numBlocks;

private:    
    Frustum* _frustum;  //for culling
    ChunkGenerator* _generator;   //for generateChunk()

    //Map of chunks, from Vector3 pos to pointer
    std::map<Vector3, Chunk*> _chunks;
    std::map <Vector3, Chunk*>::iterator _it;

    //Block types:
    //Hash from block type to values in struct
    QHash<char, blockType> _types;
    //Call in constructor to create default types
    void loadTypes();
    //helper for loadTypes()- loads an individual block type
    void loadType(char c, Vector2 tex [], bool p, bool t);

    //For automatic loading/ unloading
    Entity* _focal;  //around which chunks are loaded/ unloaded
    Vector3 _currentFocus;  //current focus
    static const int _range = 2;
    std::deque <Vector3> _toLoad;
    std::deque <Vector3> _toUnload;

    //Helpers for locating block/ chunk, based on absolute position:
    //given absolute index, find chunk it'll be in
    Vector3 inChunk(const Vector3& pos);
    //given absolute index, find block pos (relative to containing chunk)
    Vector3 inBlock(const Vector3& pos);
    //given absolute index, return chunk it'll be in, if it exists
    Chunk* getChunk(const Vector3& pos);
    Chunk* getChunkRelative(const Vector3& pos);

    //Helpers to generate world: use relative (1 1 1) coordinates
    void addChunk(Chunk* c);
    //Call to initially create chunk: sets transparency + inits vbo
    void generateChunk(const Vector3& chunkPos);

    //Helper for collide(): returns true if blocks ahead are clear
    bool checkBlocks(v3_pointer p1, v3_pointer p2, v3_pointer p3,
               Vector3& _pos,Vector3& _dim, float next);
    //Helper for unload: true if c is more than r from v in any axis
    bool outOfRange(const Vector3& c, const Vector3& v, int r);
    //Called (when _focal moves outside of _currentFocus) to calculate
    //what chunks should be added/ deleted
    void addToLoad();
    void addToUnload();

};

#endif // VOXELMANAGER_H

