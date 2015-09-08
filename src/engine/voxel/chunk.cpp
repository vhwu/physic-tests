#include "chunk.h"
#include "block.h"
#include "engine/common/graphic.h"
#include <iostream>

Chunk::Chunk(const Vector3& pos, VoxelManager* m)
{
    _pos = pos;
    _manager = m;
    _vbo = new VBO();

    //assign corners for calculating culling
    Vector3 start = Vector3(pos.x*_dim,pos.y*_dim,pos.z*_dim);
    _corners[0]=start;
    _corners[1]=start+Vector3(_dim,0,0);
    _corners[2]=start+Vector3(_dim,_dim,0);
    _corners[3]=start+Vector3(_dim,_dim,_dim);
    _corners[4]=start+Vector3(0,_dim,0);
    _corners[5]=start+Vector3(0,_dim,_dim);
    _corners[6]=start+Vector3(0,0,_dim);
    _corners[7]=start+Vector3(_dim,0,_dim);

    //for(int i=0; i<(_dim*_dim*_dim);i++)
    //    _blocks[i]=NULL;
}

Chunk::~Chunk()
{
    delete _vbo;
    for(int i=0; i<(_dim*_dim*_dim);i++){
        if(_blocks[i]!=NULL)
            delete _blocks[i];
    }
}

void Chunk::onTick(float seconds){
}

void Chunk::onDraw(Graphic* g){
    _vbo->draw();
    /*
    for(int i=0;i<_dim;i++){
          for(int j=0;j<_dim;j++){
              for(int k=0;k<_dim;k++){
                  int index = constants::getIndex(Vector3(k,j,i), _dim);
                  Block* block = _blocks[index];
                  if(block!=NULL){
                      //find absolute position by adding chunk to block translation
                      Vector3 transform = Vector3(_dim*_pos.x+k, _dim*_pos.y+j,_dim*_pos.z+i);
                      g->translate(&Graphic::drawUnitCube, transform);
                  }
              }
          }
      }
    */
}

//@param pos: position of block within chunk, not within absolute space
void Chunk::addBlock(const Vector3& pos, char type){
    int index = constants::getIndex(pos, _dim);
    if(_blocks[index]==NULL)
        _blocks[index] = new Block(type);
    else
        std::cout<<"Block already existing in chunk"<<std::endl;
}

void Chunk::removeBlock(const Vector3& pos){
    int index = constants::getIndex(pos, _dim);
    if(_blocks[index]!=NULL){
        delete _blocks[index];
        _blocks[index] = NULL;
    }
}

//Call once, after initial generation + initGL
void Chunk::initVbo(){
    _vbo->initialize();
}

//Call after procedural generation to mark unseen faces (won't be rendered)
void Chunk::setFaceTransp(){
    for(int i=0;i<_dim;i++){
        for(int j=0;j<_dim;j++){
            for(int k=0;k<_dim;k++){
                int index = constants::getIndex(Vector3(k,j,i), _dim);
                Block* block = _blocks[index];
                if(block!=NULL){
                    if(i>0) //check back
                        block->visible[BACK]=checkTransp(Vector3(k,j,i-1));
                    if(i<_dim-1)  //check front
                            block->visible[FRONT]=checkTransp(Vector3(k,j,i+1));
                    if(j>0)  //check bot
                            block->visible[BOT]=checkTransp(Vector3(k,j-1,i));
                    if(j<_dim-1) //check top
                            block->visible[TOP]=checkTransp(Vector3(k,j+1,i));
                    if(k>0) //check left
                            block->visible[LEFT]=checkTransp(Vector3(k-1,j,i));
                    if(k<_dim-1)  //check right
                            block->visible[RIGHT]=checkTransp(Vector3(k+1,j,i));
                    }
             }
        }
    }
}

//Call after procedural generation + setFaceTransp to store vertices
void Chunk::resetVbo(){
    _vbo->count = 0;

    for(int i=0;i<_dim;i++){
        for(int j=0;j<_dim;j++){
            for(int k=0;k<_dim;k++){
                int index = constants::getIndex(Vector3(k,j,i), _dim);
                Block* block = _blocks[index];
                if(block!=NULL){
                    VoxelManager::blockType b = _manager->getType(block->type);
                    Vector3 pos = Vector3(_pos.x*constants::_dim+k, _pos.y*constants::_dim+j,
                                          _pos.z*constants::_dim+i);
                    for(int f = 0; f<6; f++){
                        if(block->visible[f])
                            _vbo->addFace(pos,constants::_faces[f]._points,b._textures[f]);
                    }
                }
            }
        }
    }
}

//Tests to see if index has opaque block (false) or is empty/ transparent (true)
//@param index: position of block within chunk, not within absolute space
bool Chunk::checkTransp(const Vector3& index){
    Block* block = _blocks[constants::getIndex(index, _dim)];
    if(block!=NULL&&!(_manager->getType(block)._trans))
        return false;
    else
        return true;
}

//Tests to see if index has solid block (false) of if it's empty/ passable (true)
//@param index: position of block within chunk, not within absolute space
bool Chunk::checkPass(const Vector3& index){
    Block* block = _blocks[constants::getIndex(index, _dim)];
    if(block!=NULL&&!(_manager->getType(block)._pass))
        return false;
    else
        return true;
}

void Chunk::fill(){
    if(_pos.y<0){
        for(int i=0; i<_dim;i++){
            for(int j=0; j<_dim;j++){
                for(int k=0; k<_dim;k++)
                    addBlock(Vector3(k,j,i),'g');
            }
        }
    }
}

void Chunk::setVbo(){
    _vbo->count = 0;

    for(int i=0;i<_dim;i++){
        for(int j=0;j<_dim;j++){
            for(int k=0;k<_dim;k++){
                int index = constants::getIndex(Vector3(k,j,i), _dim);
                Block* block = _blocks[index];
                if(block!=NULL){
                    VoxelManager::blockType b = _manager->getType(block->type);
                    Vector3 pos = Vector3(_pos.x*constants::_dim+k, _pos.y*constants::_dim+j,
                                          _pos.z*constants::_dim+i);
                    if(i==0 || (i>0 && checkTransp(Vector3(k,j,i-1))))
                        _vbo->addFace(pos,constants::_faces[BACK]._points,b._textures[BACK]);
                    if(i==_dim-1 || (i<_dim-1 && checkTransp(Vector3(k,j,i+1))))
                       _vbo->addFace(pos,constants::_faces[FRONT]._points,b._textures[FRONT]);
                    if(j==0 || (j>0 && checkTransp(Vector3(k,j-1,i))))
                        _vbo->addFace(pos,constants::_faces[BOT]._points,b._textures[BOT]);
                    if(j==_dim-1 || (j<_dim-1 && checkTransp(Vector3(k,j+1,i))))
                        _vbo->addFace(pos,constants::_faces[TOP]._points,b._textures[TOP]);
                    if(k==0 || (k>0 && checkTransp(Vector3(k-1,j,i))))
                        _vbo->addFace(pos,constants::_faces[LEFT]._points,b._textures[LEFT]);
                    if(k==_dim-1 || (k<_dim-1 && checkTransp(Vector3(k+1,j,i))))
                        _vbo->addFace(pos,constants::_faces[RIGHT]._points,b._textures[RIGHT]);
                }
             }
        }
    }
}

