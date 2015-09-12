#ifndef GRAPHIC_H
#define GRAPHIC_H
#include "vector.h"
#include <QHash>
#include <QString>
#include "axis.h"

class View;
class GLUquadric;

//Loads, stores, draws, deletes all texture ids
//Held in application
//Call loadResource within application's initializeGL
class Graphic
{
public:
    Graphic(View* v);
    virtual ~Graphic();

    //********************
    Vector3 unproject(float winX, float winY);
    //UI
    void storeMatrix(); //to update matrices - call by corresponding method in app
    void drawUIBox(const Vector3& pos, const Vector2& min, const Vector2& max);

    //*include in application's initializeGL
    //Load all textures here into map by listing relevant files
    //Load sheet for texture atlas here
    void loadResource();

    void setColor(const Vector3& color);
    void setColor(const Vector4& color);
    void setLineWidth(float l);  //line width

    //Bracket call to draw something
    void bindTexture(QString s);
    void unbindTexture();
    void enableOffset(float offset);  //postive pushes it away, negative pulls it towards you
    void disableOffset();

    //Compositing modes
    void depth(bool d); //enabling/ disabling GL_DEPTH_TEST
    void opaque(bool o); //drawing opaque objects, vs. blending translucent objects
    void lit(bool l);  //enabling/ disabling GL_LIGHTING
    void cull(bool c);  //enabling/ disabling GL_CULL_FACE

    //Type for drawing unit shapes, to be passed as parameter
    //to transform methods below
    typedef void (Graphic::*drawUnitShape_pntr)();
    void drawUnitSphere();
    void drawTriangle(Vector3* vertices, const Vector3& normal);
    void drawTriangle(Vector3* vertices, Vector3* normals);

    //To draw and transform unit shape
    void transform(drawUnitShape_pntr m, const Vector3& trans,
                   int rot, const Vector3& scale);
    void translate(drawUnitShape_pntr m, const Vector3& trans);

    //SHEET/ ATLAS, for minecraft
    void bindSheet(); //call once, in manager. Must have called loadSheet()

    //To draw lines
    void drawLine(const Vector3& a, const Vector3& b);
    void drawGradientLine(const Vector3& c1, const Vector3& c2,
                          const Vector3& a, const Vector3& b);
    void drawCircle(Axis a, float radius, const Vector3& center);

protected:
    View* _view;
private:
    QHash<QString, int> _textures; //map of textures to their IDs
    int _sheet; //id of sprite sheet

    //helper for loadResource- loads single texture, returns ID
    int loadTexture(const char* texture_name);
    //helper for loadResource- loads in sprite sheet, sets sheet
    void loadSheet();

    GLUquadric* quadObj;  //for spheres/ cylinders

    //for UI element projection calculations
    double modelMatrix[16], projMatrix[16];
    int viewMatrix[4];
    void to2D(const Vector3& pos); //use within drawUI calls for projection
};

#endif // GRAPHIC_H
