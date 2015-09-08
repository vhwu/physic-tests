#ifndef VBO_H
#define VBO_H

#include "vector.h"
#include "constants.h"
#if defined(__APPLE__) || defined(MACOSX)
    #include <OpenGL/glu.h>
#else
    #define GL_GLEXT_PROTOTYPES
    #include <GL/glu.h>
#endif

//Arrays of rendering data kept in GPU, so that information doesn't need to
//be sent from the CPU on each draw()
//Array of structs: interleaving improves static object speed
//Struct of arrays: good for repeatedly modifying only some attributes
class VBO
{
public:
    VBO();
    ~VBO();

    #pragma pack(push,1)
    struct vertex{
        float _pos[3];
        float _nor[3];
        float _tex[2];
        vertex(){}
        vertex(float px,float py, float pz,
               float nx, float ny, float nz, float t1, float t2){
            _pos[0]=px;_pos[1]=py;_pos[2]=pz;
            _nor[0]=nx;_nor[1]=ny;_nor[2]=nz;
            _tex[0]=t1;_tex[1]=t2;
        }
    };
    #pragma pack(pop)

    //Given a face's position (of containing block), corner offets, + texture coord,
    //add its 4 vertices to the VBO
    void addFace(const Vector3& pos, const Vector3 (&face)[4], const Vector2& tex);

    //Call after initGL and containing Chunk has input data into vertices (resetVBO)
    void initialize();

    void draw();
//private:
    //Contains information for each vertex of each visible face of each block
    vertex _vertices[24*constants::_dim*constants::_dim*constants::_dim]={};
    unsigned int id;   //to bind buffer
    int count;  //number of vertices
};

#endif // VBO_H
