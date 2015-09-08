#include "graphic.h"
#include "view.h"
#include <iostream>

#if defined(__APPLE__) || defined(MACOSX)
    #include <OpenGL/glu.h>
    #include <OpenGL/gl.h>
#else
    #include <GL/glu.h>
#endif

Graphic::Graphic(View* v)
{
    _view = v;
    quadObj = gluNewQuadric();
}

Graphic::~Graphic()
{
    _textures.clear();
}

Vector3 Graphic::unproject(float winX, float winY)
{
    //invert y, since openGL starts on bottom left corner
    float y = viewMatrix[3]-winY;

    double objX, objY, objZ;
    //Returns point on far plane- for near plane, replace 1 w/ 0
    gluUnProject(winX, y, 1, modelMatrix, projMatrix, viewMatrix, &objX, &objY, &objZ);

    return Vector3(objX, objY, objZ);
}


void Graphic::storeMatrix(){
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    glGetIntegerv(GL_VIEWPORT, viewMatrix);
}

void Graphic::to2D(const Vector3& pos){
    double sx, sy, sz; //screen-space coordinates
    gluProject(pos.x, pos.y, pos.z, modelMatrix, projMatrix, viewMatrix, &sx, &sy, &sz);
    glPushMatrix();
    glTranslatef( sx, sy, 0 );
}

void Graphic::drawUIBox(const Vector3& pos, const Vector2& min, const Vector2& max){
    to2D(pos);
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glVertex2f(min.x, min.y);
    glVertex2f(max.x, min.y);
    glVertex2f(max.x, max.y);
    glVertex2f(min.x, max.y);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();
}

//Include calls to loadTexture and loadSheet for necessary textures
void Graphic::loadResource(){
    _textures["mountain"] = loadTexture(":images/level/level_mountain.png");
    _textures["lowpoly"] = loadTexture(":images/level/level_lowpoly.png");
    _textures["one"] = loadTexture(":images/level/level_one.png");

    loadSheet();
}

int Graphic::loadTexture(const char* texture_name){
    QImage *image = new QImage(texture_name);
    //GL stores image differently than QT, so flip
    *image = image->mirrored(false,true);
    //View extends from QGLWidget = convenience method for binding textures
    int id = _view->bindTexture(*image, GL_TEXTURE_2D, GL_RGBA, QGLContext::MipmapBindOption);
    unbindTexture();
    delete image;
    return id;
}

void Graphic::loadSheet(){
    _sheet = loadTexture(":images/terrain.png");
}

void Graphic::bindSheet(){
    glBindTexture(GL_TEXTURE_2D, _sheet);
    glEnable(GL_TEXTURE_2D);
    setColor(Vector3(1, 1, 1));
}

void Graphic::bindTexture(QString s){
    if(_textures.contains(s))
        glBindTexture(GL_TEXTURE_2D, _textures.value(s));
    glEnable(GL_TEXTURE_2D);
}

 void Graphic::unbindTexture(){
    glDisable(GL_TEXTURE_2D);
}

 void Graphic::enableOffset(float offset){
     glEnable(GL_POLYGON_OFFSET_FILL);
     glPolygonOffset(offset,.5);
 }

 void Graphic::disableOffset(){
     glDisable(GL_POLYGON_OFFSET_FILL);

 }

void Graphic::setColor(const Vector3& color){
    glColor3f(color.x, color.y, color.z);
}

void Graphic::setColor(const Vector4& color){
    glColor4f(color.x, color.y, color.z, color.w);
}

void Graphic::setLineWidth(float l){
    glLineWidth(l);
}

void Graphic::depth(bool d){
    if(d)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void Graphic::opaque(bool o){
    if(o)
        glDepthMask(GL_TRUE);
    else
        glDepthMask(GL_FALSE);
}

void Graphic::cull(bool c){
    if(c)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
}

void Graphic::drawLine(const Vector3& a, const Vector3& b){
    glBegin(GL_LINES);
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glEnd();
}

void Graphic::drawGradientLine(const Vector3& c1, const Vector3& c2,
                               const Vector3& a, const Vector3& b){
    glBegin(GL_LINES);
    setColor(c1);
    glVertex3f(a.x, a.y, a.z);
    setColor(c2);
    glVertex3f(b.x, b.y, b.z);
    glEnd();
}

void Graphic::drawUnitSphere(){
    //GLUquadric *quadObj = gluNewQuadric();
    //radius top, radius bot, height, tesselation, slices?
    gluSphere(quadObj, 1, 32, 16);
}

void Graphic::lit(bool l){
    if(l)
        glEnable(GL_LIGHTING);
    else
        glDisable(GL_LIGHTING);
}

void Graphic::drawTriangle(Vector3* vertices, const Vector3& normal){
    glNormal3fv(normal.xyz);
    glBegin(GL_TRIANGLES);
    glVertex3fv(vertices[0].xyz);
    glVertex3fv(vertices[1].xyz);
    glVertex3fv(vertices[2].xyz);
    glEnd();
}

void Graphic::drawTriangle(Vector3* vertices, Vector3* normals){
    glBegin(GL_TRIANGLES);
    glNormal3fv(normals[0].xyz);
    glVertex3fv(vertices[0].xyz);
    glNormal3fv(normals[1].xyz);
    glVertex3fv(vertices[1].xyz);
    glNormal3fv(normals[2].xyz);
    glVertex3fv(vertices[2].xyz);
    glEnd();
}

//@param m: a method to draw a unit shape- no args
//@param trans, rot, scale: transformations applied to primitive
 void Graphic::transform(drawUnitShape_pntr m, const Vector3& trans,
                         int rot, const Vector3& scale){
    glPushMatrix();
    glTranslatef(trans.x,trans.y,trans.z);
    Vector3 vec = Vector3(-1,1,1);
    vec.normalize();
    glRotatef(rot, vec.x, vec.y, vec.z);
    glScalef(scale.x,scale.y,scale.z);

    (this->*m)();
    glPopMatrix();
}

 //@param m: a method to draw a unit shape- no args
 //@param trans: translation applied to primitive
 void Graphic::translate(drawUnitShape_pntr m, const Vector3& trans){
    glPushMatrix();
    glTranslatef(trans.x,trans.y,trans.z);
    (this->*m)();
    glPopMatrix();
}



