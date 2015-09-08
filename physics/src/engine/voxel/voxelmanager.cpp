#include "voxelmanager.h"
#include "chunk.h"
#include "block.h"
#include "vbo.h"
#include "chunkgenerator.h"
#include "engine/common/graphic.h"
#include "engine/common/entity.h"
#include "engine/common/frustum.h"
#include "engine/common/entity.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <limits>

using constants::getFloating;
using constants::pY;
using constants::pX;
using constants::pZ;

VoxelManager::VoxelManager(Frustum* f, Entity* e, ChunkGenerator *g)
{
    _numBlocks = 0;

    _frustum = f;
    _focal = e;
    _generator = g;

    //Load block types
    loadTypes();
}

VoxelManager::~VoxelManager()
{
    for(_it = _chunks.begin(); _it!=_chunks.end(); ++_it)
        delete _it->second;
    _chunks.clear();
}

void VoxelManager::initialize(){
    //Set focus for generation
    _currentFocus = inChunk( _focal->getPos());

    //Create all chunks within _range
    Vector3 start = _currentFocus-_range;
    Vector3 end = _currentFocus+_range+1;

    for(int i = start.z; i<end.z; i++){
        for(int j = start.y; j<end.y; j++){
            for(int k = start.x; k<end.x; k++)
                generateChunk(Vector3(k,j,i));
        }
    }
}

/****************************TYPES***************************/
void VoxelManager::loadTypes(){
    //Default dirt type. Inserted if block has type that doesn't exist
    Vector2 _v = Vector2(2,15);
    Vector2 array [6] = {_v,_v,_v,_v,_v,_v};
    loadType('d', array, false, false);

    //Dirt topped with grass
    Vector2 grass [6];
    grass[0]= Vector2(2,15);
    grass[1]= Vector2(0,15);
    grass[2]= Vector2(3,15);
    grass[3]= Vector2(3,15);
    grass[4]= Vector2(3,15);
    grass[5]= Vector2(3,15);

    loadType('g', grass, false, false);

    //Dirt topped with snow
    Vector2 snow [6];
    snow[0]= Vector2(2,15);
    snow[1]= Vector2(2,11);
    snow[2]= Vector2(4,11);
    snow[3]= Vector2(4,11);
    snow[4]= Vector2(4,11);
    snow[5]= Vector2(4,11);

    loadType('s', snow, false, false);
}

void VoxelManager::loadType(char c, Vector2 tex [], bool p, bool t){
    Vector2* _textures = new Vector2[6];

    //Convert from given coordinates to scaled coordinates in sheet
    for(int i = 0; i<6; i++){
        _textures[i]=constants::getSheetCoor(tex[i]);
    }

    blockType b = {_textures,p,t};
    _types[c] = b;
}

//Returns default type if it isn't found
VoxelManager::blockType VoxelManager::getType(char c){
    if(_types.contains(c))
        return _types.value(c);
    else
        return _types.value('d');
}
VoxelManager::blockType VoxelManager::getType(Block* b){
    return getType(b->type);
}

/****************TERRAIN GENERATION/ UPDATES*******************/
//Called by world/ _generator
//@param pos: in absolute coordinates (ex: 33 55 77)
void VoxelManager::addBlock(char c, const Vector3& pos){
    _numBlocks++;
    Chunk* chunk = getChunk(pos);
    if(chunk!=NULL)
        chunk->addBlock(inBlock(pos),c);
    else
        std::cout<<"Chunk doesn't yet exist for:"<<pos<<std::endl;
}

bool VoxelManager::checkBlock(const Vector3& nextPos){
    Chunk* c = getChunk(nextPos);
    if(c!=NULL)
        return c->checkPass(inBlock(nextPos));
    else
        return true;

}

void VoxelManager::removeBlock(const Vector3& pos){
    _numBlocks--;
    Chunk* chunk = getChunk(pos);
    if(chunk!=NULL)
        chunk->removeBlock(inBlock(pos));
    else
        std::cout<<"Chunk doesn't yet exist for:"<<pos<<std::endl;
}

//@param chunkPos: in relative coordinates
void VoxelManager::updateChunk(const Vector3& chunkPos){
    Chunk *c = getChunk(chunkPos);
    if(c!=NULL){
        c->setFaceTransp();
        c->resetVbo();
    }
}

//Called by manager itself
void VoxelManager::addChunk(Chunk* c){
    Vector3 pos = c->getPos();
    std::map<Vector3, Chunk*>::iterator i = _chunks.find(pos);
    if(i!=_chunks.end())
        std::cout<<"Chunk already exists:"<<pos<<std::endl;
    else
        _chunks[pos] = c;
}

void VoxelManager::generateChunk(const Vector3& chunkPos){
    //Creates chunk and adds it to _chunks
    Chunk* c = new Chunk(chunkPos, this);
    addChunk(c);
    //c->fill();
    //Procedural terrain generation
    _generator->generateChunk(chunkPos);
    //Loading information into chunk's vbo
    c->setFaceTransp();
    c->resetVbo();
    c->initVbo();
}

/*******************ADDING / ACCESSING BLOCKS/ CHUNKS******************/
//Helper: given absolute coord, find chunk position (1 1 1) it'll be in
Vector3 VoxelManager::inChunk(const Vector3& pos){
    int _bDim = constants::_dim;
    //For negatives, which has ranges like -32 to -1:
    //pos+1: so that edge cases (ex:-32, -31) aren't in different chunks after division
        //Default: -32 / 32 = -1, -31 / 32 = 0, when both need to be 0
    //-1: to 'round down' index
        //Default: -31/32 = 0, when it needs to be at -1
    int x = (pos.x>-1) ? ((int)pos.x)/_bDim : (((int)pos.x+1)/_bDim)-1;
    int y = (pos.y>-1) ? ((int)pos.y)/_bDim : (((int)pos.y+1)/_bDim)-1;
    int z = (pos.z>-1) ? ((int)pos.z)/_bDim : (((int)pos.z+1)/_bDim)-1;

    return Vector3(x,y,z);
}

//Helper: given absolute coord, find block position (relative to chunk, not absolute)
Vector3 VoxelManager::inBlock(const Vector3& pos){
    int _bDim = constants::_dim;
    int x = ((int)pos.x)%_bDim;
        if(x<0) x+=_bDim;  //-1%32=-1, when it should be 31
    int y = ((int)pos.y)%_bDim;
        if(y<0) y+=_bDim;
    int z = ((int)pos.z)%_bDim;
        if(z<0) z+=_bDim;
    return Vector3(x,y,z);
}

//Helper: given absolute coord, return chunk if it exists there
Chunk* VoxelManager::getChunk(const Vector3& pos){
    Vector3 chunk_pos = inChunk(pos);
    std::map<Vector3, Chunk*>::iterator i = _chunks.find(chunk_pos);
    if(i!=_chunks.end())
        return i->second;
    else
        return NULL;
}

//Helper: given relative coord, return chunk if it exists there
Chunk* VoxelManager::getChunkRelative(const Vector3& pos){
    std::map<Vector3, Chunk*>::iterator i = _chunks.find(pos);
    if(i!=_chunks.end())
        return i->second;
    else
        return NULL;
}

/****************************LOOP THROUGH***************************/
void VoxelManager::onTick(float seconds)
{
    //Update focus and add to load/unLoad if it changed
    Vector3 p = inChunk(_focal->getPos());
    if(_currentFocus!=p){
        _currentFocus=p;
        addToLoad();
        addToUnload();
    }
    //If there's chunks to be loaded, load one
    if(!_toLoad.empty()){
        Vector3 c = _toLoad.front();
        //check to see if it's on unload deque
        if(std::find(_toUnload.begin(), _toUnload.end(), c) == _toUnload.end())
             generateChunk(c);
        _toLoad.pop_front();
    }
    //If there's chunks to be unloaded, unload one
    if(!_toUnload.empty()){
        Vector3 c = _toUnload.front();
        //chck to see if it's on load deque
        if(std::find(_toLoad.begin(), _toLoad.end(), c) == _toLoad.end())
            _chunks.erase(c);
        _toUnload.pop_front();
    }
}

void VoxelManager::onDraw(Graphic* g){
    g->bindSheet();
    for(_it = _chunks.begin(); _it!=_chunks.end(); ++_it){
        Chunk* c = _it->second;
        if(!_frustum->cullAABB(c->_corners))
            c->onDraw(g);
    }
    g->unbindTexture();
}

Vector3 VoxelManager::collide(Entity* e){
    Vector3 pos = e->getPos();
    Vector3 dim = e->getDim();
    Vector3 move = e->getMove();


    //adjusted velocity, w/ components set to 0 if there's a collision
    Vector3 newVel = e->getVel();
    Vector3 mtv = Vector3(0,0,0);

    //Piecewise information
    struct SweepInfo{
        float move;     //absolute value of movement
        int step;       //whether move is positive or negative (1 or -1)
        float pos;      //End of cube used in calculations (max or min), given direction
        float dist;     //Distance to next block boundary- must be <1
        int next;       //Index of next block boundary that would be crossed

        SweepInfo(float _move, float _pos, float _dim){
            move = fabs(_move);
            step = (_move>0) ? 1 : -1;
            pos = (_move>0) ? _pos+_dim : _pos;
            dist = ((_move>0&&pos>0)||(_move<0&&pos<0)) ?
                        getFloating(1.0-fabs(getFloating(pos))):fabs(getFloating(pos));
            next = (_move>0) ? ceil(pos) : floor(pos)-1;
           }

        //Mtv set to overlap, move set to reach only next block, vel set to 0
        void resolve(Vector3& _move, Vector3& _vel, Vector3& _mtv, v3_pointer p){
            _mtv.*p = step*(move-dist);
            _vel.*p = 0;
            _move.*p = step * (dist - std::numeric_limits<float>::epsilon());
        }
    };

    SweepInfo sweepY = SweepInfo(move.y,pos.y,dim.y);
    //if move will pass boundary and there are blocks in the way
    if(sweepY.move>sweepY.dist && !checkBlocks(pY, pX, pZ, pos, dim, sweepY.next))
            sweepY.resolve(move, newVel, mtv, pY);
    e->move(Vector3(0,move.y,0));  //move one axis at a time
    pos = e->getPos();  //reset position for next calculation

    SweepInfo sweepX = SweepInfo(move.x,pos.x,dim.x);
    if(sweepX.move>sweepX.dist && !checkBlocks(pX, pY, pZ, pos, dim, sweepX.next)) //if collide with blocks
            sweepX.resolve(move, newVel, mtv, pX);
    e->move(Vector3(move.x,0,0));
    pos = e->getPos();

    SweepInfo sweepZ = SweepInfo(move.z,pos.z,dim.z);
    if(sweepZ.move>sweepZ.dist && !checkBlocks(pZ, pX, pY, pos, dim, sweepZ.next)) //if collide with blocks
            sweepZ.resolve(move, newVel, mtv, pZ);
    e->move(Vector3(0,0,move.z));

    e->setMove(Vector3(0,0,0));
    e->setVel(newVel);

    return mtv;
}

/************************HELPERS***********************/
//For collision
//@param p1, p2, p3: p1 is the direction of movement (axis) to check in
//@param _pos, _dim: of colliding entity
//@param next: next index (in p1) to check
bool VoxelManager::checkBlocks(v3_pointer p1, v3_pointer p2, v3_pointer p3,
                         Vector3& _pos,Vector3& _dim,float next){
    std::vector<Vector3> blocks;
    //grid of blocks that entity will potentially move into
    for(int i = floor(_pos.*p2); i<_pos.*p2+_dim.*p2; i++){
        for(int j = floor(_pos.*p3); j<_pos.*p3+_dim.*p3; j++){
            Vector3 v; v.*p1=next; v.*p2=i; v.*p3=j;
            blocks.push_back(v);
        }
    }

    bool clear = true;
    for(int i = 0; i<blocks.size(); i++)
        clear = clear && checkBlock(blocks[i]);
    return clear;
}

//For addToUnload
bool VoxelManager::outOfRange(const Vector3& c, const Vector3& v, int r){
    int diffX = (c.x>v.x) ? c.x-v.x : v.x-c.x;
    if((abs((int)diffX))>r)
        return true;
    int diffY = (c.y>v.y) ? c.y-v.y : v.y-c.y;
    if((abs((int)diffY))>r)
        return true;
    int diffZ = (c.z>v.z) ? c.z-v.z : v.z-c.z;
    if((abs((int)diffZ))>r)
        return true;
    return false;
}

//Helpers for chunk streaming: called onTick
void VoxelManager::addToLoad(){
    Vector3 start = _currentFocus-_range;
    Vector3 end = _currentFocus+_range+1;

    for(int i = start.z; i<end.z; i++){
        for(int j = start.y; j<end.y; j++){
            for(int k = start.x; k<end.x; k++){
                Vector3 v = Vector3(k,j,i);
                //if it doesn't already exist and isn't already going to be added
                if(getChunkRelative(v)==NULL &&
                        std::find(_toLoad.begin(), _toLoad.end(), v) == _toLoad.end())
                    _toLoad.push_back(v);
            }
        }
    }
}

void VoxelManager::addToUnload(){
    for(_it = _chunks.begin(); _it!=_chunks.end(); ++_it){
        //test each existing chunk to see if it is out of range
        Vector3 v = _it->second->getPos();
        if(outOfRange(v, _currentFocus, _range+1))
            _toUnload.push_back(v);
    }
}


