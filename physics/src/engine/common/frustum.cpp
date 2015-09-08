#include "frustum.h"
#include "constants.h"
#if defined(__APPLE__) || defined(MACOSX)
    #include <OpenGL/glu.h>
#else
    #include <GL/glu.h>
#endif

Frustum::Frustum()
{}

Frustum::~Frustum()
{}

//@param: res = a (model view) * b (projection)
void Frustum::multMat(float *res, float *a, float *b){
    glPushMatrix();
    glLoadMatrixf(b);
    glMultMatrixf(a);
    glGetFloatv(GL_MODELVIEW_MATRIX, res);
    glPopMatrix;
}

//@param: OpenGL ModelView and Projection matrices, from glGetFloatv
void Frustum::update(float* model, float* project){
    //get product (m)
    float m[16];
    multMat(m, model, project);

    //left
    _planes[LEFT]= Vector4(
                m[3]-m[0],
                m[7]-m[4],
                m[11]-m[8],
                m[15]-m[12]);
    //bot
    _planes[BOT]= Vector4(
                m[3]-m[1],
                m[7]-m[5],
                m[11]-m[9],
                m[15]-m[13]);
    //far
    _planes[BACK]= Vector4(
                m[3]-m[2],
                m[7]-m[6],
                m[11]-m[10],
                m[15]-m[14]);
    //right
    _planes[RIGHT]= Vector4(
                m[3]+m[0],
                m[7]+m[4],
                m[11]+m[8],
                m[15]+m[12]);
    //top
    _planes[TOP]= Vector4(
                m[3]+m[1],
                m[7]+m[5],
                m[11]+m[9],
                m[15]+m[13]);
    //near
    _planes[FRONT]= Vector4(
                m[3]+m[2],
                m[7]+m[6],
                m[11]+m[10],
                m[15]+m[14]);
}

bool Frustum::cullPoint(const Vector3& v, const Vector4& plane){
    return ((v.x*plane.x+v.y*plane.y+v.z*plane.z+plane.w)<0);
}

bool Frustum::cullPlane(Vector3 (&points)[8], const Vector4& plane){
    bool behind = true;
    for(int i = 0; (i<8&&behind); i++){
        Vector3 p = points[i];
        if (!this->cullPoint(p,plane))
            behind = false;
    }
    return behind;
}

bool Frustum::cullAABB(Vector3 (&points)[8]){
    bool inFront = true;
    for(int i = 0; (i<6&&inFront); i++){
        if (cullPlane(points, _planes[i]))
            inFront = false;
    }
    return !inFront;
}


