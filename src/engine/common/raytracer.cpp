#include "raytracer.h"
#include "ellipsoid.h"
#include "engine/geometric/triangle.h"
#include "engine/verlet/verlet.h"
#include "engine/verlet/constraint.h"

#include <qgl.h>
#if defined(__APPLE__) || defined(MACOSX)
    #include <OpenGL/glu.h>
#else
    #include <GL/glu.h>
#endif

static Vector3 unProject(float winX, float winY, double *modelview, double *projection, int *viewport)
{
    double objX, objY, objZ;
    gluUnProject(winX, winY, 1, modelview, projection, viewport, &objX, &objY, &objZ);
    return Vector3(objX, objY, objZ);
}

RayTracer::RayTracer(const Vector3& s, const Vector3& d){
    source = s;
    direction = d;
}

// Read in the current modelview and projection matrices and remember them
RayTracer::RayTracer(double modelview[16], int mouseX, int mouseY)
{    
    //Mac doesn't seem to return the right values

    #if defined(__APPLE__) || defined(MACOSX)
        viewport[0]=0;
        viewport[1]=0;
        viewport[2]=800;
        viewport[3]=600;
    #else
        glGetIntegerv(GL_VIEWPORT, viewport);
    #endif
    update(modelview, mouseX, mouseY);

}

void RayTracer::update(double modelview[], int mouseX, int mouseY){
    //glGetIntegerv(GL_VIEWPORT, viewport);
    // Read camera information from OpenGL
    double projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projection);

    // Reconstruct the eye position
    Vector3 xaxis(modelview[0], modelview[1], modelview[2]);
    Vector3 yaxis(modelview[4], modelview[5], modelview[6]);
    Vector3 zaxis(modelview[8], modelview[9], modelview[10]);
    Vector3 offset(modelview[12], modelview[13], modelview[14]);
    // Stores the origin in world space at the time of the constructor
    source = -Vector3(offset.dot(xaxis), offset.dot(yaxis), offset.dot(zaxis));

    // Generate the four corner rays
    int xmin = viewport[0];
    int ymin = viewport[1];
    int xmax = xmin + viewport[2] - 1;
    int ymax = ymin + viewport[3] - 1;
    ray00 = unProject(xmin, ymin, modelview, projection, viewport) - source;
    ray10 = unProject(xmax, ymin, modelview, projection, viewport) - source;
    ray01 = unProject(xmin, ymax, modelview, projection, viewport) - source;
    ray11 = unProject(xmax, ymax, modelview, projection, viewport) - source;

    direction = getRayForPixel(mouseX, mouseY);
}

RayTracer::~RayTracer(){

}

Vector3 RayTracer::getRayForPixel(int x, int y) const
{
    // Construct the per-pixel ray using linear interpolation from the rays
    // along each corner of the frustum (the origin is the eye point: source)

    float fx = (float)(x - viewport[0]) / (float)viewport[2];
    float fy = 1 - (float)(y - viewport[1]) / (float)viewport[3];
    Vector3 ray0 = Vector3::lerp(ray00, ray10, fx);
    Vector3 ray1 = Vector3::lerp(ray01, ray11, fx);
    return Vector3::lerp(ray0, ray1, fy).unit();
}

float RayTracer::hitPlane(const Vector3& point, const Vector3& normal){
    float denom = normal.dot(direction);
    float t = (point-source).dot(normal)/ denom;
    return t;
}

Vector3 RayTracer::getPointonPlane(const Vector3& point, const Vector3& normal){
    float t = hitPlane(point, normal);
    return getPoint(t);
}


bool RayTracer::hitTriangle(Triangle *tri, HitTest &result){
    if(tri==NULL)
        return false;

    float t = -(tri->normal.dot(source-tri->vertices[1]))/(tri->normal.dot(direction));

    if(t>0){   //check if ray points towards triangle
        Vector3 v1 = tri->vertices[0]-source;
        Vector3 v2 = tri->vertices[1]-source;
        Vector3 v3 = tri->vertices[2]-source;

        Vector3 cross1 = v1.cross(v2);
        Vector3 cross2 = v2.cross(v3);
        Vector3 cross3 = v3.cross(v1);

        //check if point where ray intersects w/ plane triangle is on, is in triangle
        if(!(cross1.dot(direction)>0) && !(cross2.dot(direction)>0) && !(cross3.dot(direction)>0)){
            result.hit = true;
            result.t = t;
            result.normal = tri->normal;
            result.tri = tri;
            return true;
        }
    }
    return false;
}

bool RayTracer::hitTriangle(Triangle tri, HitTest &result){
    float t = -(tri.normal.dot(source-tri.vertices[1]))/(tri.normal.dot(direction));

    if(t>0){   //check if ray points towards triangle
        Vector3 v1 = tri.vertices[0]-source;
        Vector3 v2 = tri.vertices[1]-source;
        Vector3 v3 = tri.vertices[2]-source;

        Vector3 cross1 = v1.cross(v2);
        Vector3 cross2 = v2.cross(v3);
        Vector3 cross3 = v3.cross(v1);

        //check if point where ray intersects w/ plane triangle is on, is in triangle
        if(!(cross1.dot(direction)>0) && !(cross2.dot(direction)>0) && !(cross3.dot(direction)>0)){
            result.hit = true;
            result.t = t;
            result.normal = tri.normal;
            //result.tri = tri;
            return true;
        }
    }
    return false;
}

bool RayTracer::hitMesh(std::vector<Triangle *> triangles, HitTest &result){
    bool h = false;
    for (Triangle* t: triangles){
        HitTest temp;
        bool hit = hitTriangle(t,temp);
        if(hit&&(temp.t<result.t)){
            h = true;
            result = temp;
        }
    }
    return h;
}

bool RayTracer::hitMesh(std::vector<Triangle> triangles, HitTest &result){
    bool h = false;
    for (Triangle t: triangles){
        HitTest temp;
        bool hit = hitTriangle(t,temp);
        if(hit&&(temp.t<result.t)){
            h = true;
            result = temp;
        }
    }
    return h;
}


bool RayTracer::hitVerlet(Verlet* verlet, HitTest &result){
    bool h = false;
    Vector3 dim = verlet->rayTraceSize;
    for(int i = 0; i<verlet->getSize(); i++){
        Vector3 pos = verlet->getPoint(i);
        HitTest temp;
        bool hit = hitEllipsoid(pos,dim,i,temp);
        if(hit&&(temp.t<result.t)){
            h = true;
            result = temp;
        }
    }
    if(h)
        result.v = verlet;
    return h;
}

bool RayTracer::hitVerlet(Verlet* verlet, QList<int> points, HitTest &result){
    bool h = false;
    for(int i = 0; i<points.size(); i++){
        Vector3 pos = verlet->getPoint(points[i]);
        HitTest temp;
        bool hit = hitEllipsoid(pos,verlet->rayTraceSize,points[i],temp);
        if(hit&&(temp.t<result.t)){
            h = true;
            result = temp;
        }
    }
    if(h)
        result.v = verlet;
    return h;
}

bool RayTracer::hitTriangle(Tri* tri, HitTest &result){
    if(tri==NULL)
        return false;

    float t = -(tri->normal.dot(source-tri->vertices[1]))/(tri->normal.dot(direction));

    if(t>0){   //check if ray points towards triangle
        Vector3 v1 = tri->vertices[0]-source;
        Vector3 v2 = tri->vertices[1]-source;
        Vector3 v3 = tri->vertices[2]-source;

        Vector3 cross1 = v1.cross(v2);
        Vector3 cross2 = v2.cross(v3);
        Vector3 cross3 = v3.cross(v1);

        //check if point where ray intersects w/ plane triangle is on, is in triangle
        if(!(cross1.dot(direction)>0) && !(cross2.dot(direction)>0) && !(cross3.dot(direction)>0)){
            result.hit = true;
            result.t = t;
            result.normal = tri->normal;
            result.verletTri = tri;
            return true;
        }
    }
    return false;
}

bool RayTracer::hitMesh(std::vector<Tri *> triangles, HitTest &result){
    bool h = false;
    for (Tri* t: triangles){
        HitTest temp;
        bool hit = hitTriangle(t,temp);
        if(hit&&(temp.t<result.t)){
            h = true;
            result = temp;
        }
    }
    return h;
}

bool RayTracer::hitConstraint(Constraint *c, HitTest &result){
    int index = c->getIndex();
    Verlet* v = c->getVerlet();
    Vector3 pos = v->getPoint(index);
    bool hit = hitEllipsoid(pos,v->rayTraceSize,index,result);
    if(hit){
        result.v = v;
        result.c = c;
    }
    return hit;
}


bool RayTracer::hitAABB(const Vector3 &cubeMin, const Vector3 &cubeMax, HitTest &result)
{
    // This uses the slab intersection method
    Vector3 tMin = (cubeMin - source) / direction;
    Vector3 tMax = (cubeMax - source) / direction;
    Vector3 t1 = Vector3::min(tMin, tMax);
    Vector3 t2 = Vector3::max(tMin, tMax);
    float tNear = t1.max();
    float tFar = t2.min();


    // Pick the intersection with the front faces (not the back faces)
    if (tNear > 0 && tNear < tFar) {

        const float epsilon = 1.0e-6;
        Vector3 hit = source + direction * tNear;
        if (hit.x < cubeMin.x + epsilon) result.normal = Vector3(-1, 0, 0);
        else if (hit.y < cubeMin.y + epsilon) result.normal = Vector3(0, -1, 0);
        else if (hit.z < cubeMin.z + epsilon) result.normal = Vector3(0, 0, -1);
        else if (hit.x > cubeMax.x - epsilon) result.normal = Vector3(1, 0, 0);
        else if (hit.y > cubeMax.y - epsilon) result.normal = Vector3(0, 1, 0);
        else result.normal = Vector3(0, 0, 1);
        result.hit = true;
        result.t = tNear;
        return true;
    }
    return false;
}

bool RayTracer::hitEllipsoid(const Vector3 &pos, const Vector3 &dim, int id, HitTest &result){
    //Translate circle center + ray source by -(circle center)
    Vector3 newSource = source-pos;
    //Scale world so ellipse is unit sphere- do not renormalize
    Vector3 basis =1.0/dim;
    newSource*=basis;
    Vector3 newDir = direction*basis;

    //Unit sphere at origin: x^2+y^2+z^2 = 1
    float a = newDir.dot(newDir);
    float b = 2*(newDir.dot(newSource));
    float c = (newSource.dot(newSource))-1;

    float t = solveQuadratic(a,b,c);

    if(t>0){
        result.hit = true;
        result.t = t;
        result.id = id;
        return true;
    }

    return false;
}

bool RayTracer::hitEllipsoid(Ellipsoid* e, HitTest &result)
{
    if(e==NULL)
        return false;

    return hitEllipsoid(e->getPos(), e->getDimension(),0,result);
}

float RayTracer::solveQuadratic(float a, float b, float c){
   float d = b*b-(4.0*a*c);

   if(d==0){   //1 intersection
       float t = -b/a;
       if(t>0)
           return t;
   }
   else if(d>0){  //2 intersections
       float square = sqrt(d);
       float r1 = (-b-square)/(2*a);
       float r2 = (-b+square)/(2*a);

       if(r1>r2){
           float temp = r2;
           r2=r1;
           r1=temp;
       }

       if(r1>0)
           return r1;
       else if(r2>0)
           return r2;
   }
   return 0;
}

