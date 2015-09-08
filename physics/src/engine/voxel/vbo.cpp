#include "vbo.h"

VBO::VBO()
{
    count = 0;
}

void VBO::draw(){
/*  Example of two quads
    vertex _v[]={
        vertex(0,0,0,  1,1,1,  0,0),
        vertex(1,0,0,  1,1,1,  0,1),
        vertex(1,0,1,  1,1,1,  1,1),
        vertex(0,0,1,  1,1,1,  1,0),
        vertex(0,1,0,  1,1,1,  0,0),
        vertex(1,1,0,  1,1,1,  0,1),
        vertex(1,1,1,  1,1,1,  1,1),
        vertex(0,1,1,  1,1,1,  1,0),
    };
*/
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

     //distance from beginning of one entity to the beginning of following entity
    unsigned int stride = sizeof(vertex);
    glVertexPointer(3, GL_FLOAT, stride, &_vertices->_pos[0]);
    glNormalPointer(GL_FLOAT, stride, &_vertices->_nor[0]);
    glTexCoordPointer(2, GL_FLOAT, stride, &_vertices->_tex[0]);

    //render primitive from array data
    //@params: mode, first, count
    glDrawArrays(GL_QUADS, 0, count);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::initialize(){
    glGenBuffers(1, &id); //generate new id

    //glBindBuffer(GL_ARRAY_BUFFER, id); //bind buffer
    //Upload the data. GL_STATIC_DRAW = buffer contents modified once,
    //glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind buffer
}

//@param pos: pos (in index of _blocks) of block that will have its face added
//@param face: position offets to get corner vertices from point. From constant's _faces
//@param tex: texture coordinate of face. From voxelManager's blockType
void VBO::addFace(const Vector3& pos, const Vector3 (&face)[4], const Vector2& tex){

    for(int i = 0; i<4; i++){
        Vector3 p1 = pos+face[i];
        Vector3 n1 = Vector3(1,1,1);
        Vector2 t1 = tex+constants::t[i];
        _vertices[count]=VBO::vertex(p1.x,p1.y,p1.z,n1.x,n1.y,n1.z,t1.x,t1.y);
        count++;
    }
}

VBO::~VBO()
{
    glDeleteBuffers(1,&id);
    id=0;
    count = 0;
}

