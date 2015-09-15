#include "cloth.h"

//Dimension: specifies the number of points in (width,height)- not number of triangles
//w: triangles are equilateral, with sides of length w
//start: position of point 0
//a: which Axis the cloth lies 'flat' along
Cloth::Cloth(const Vector2 &dimension, float w,
             const Vector3 &start, Axis a, VerletManager *vm): Verlet(vm)
{
    int r = dimension.y;
    int c = dimension.x;
    row = r;
    col = c;

    float h = (sqrt(3)/2.0) * w;

    Vector3 width;
    Vector3 height;

    if(a==X){
        width = Vector3(0,0,-w);
        height = Vector3(0,-h,0);
    }
    else if(a==Y){
        width = Vector3(0,0,-w);
        height = Vector3(-h,0,0);
    }
    else{
        width = Vector3(-w,0,0);
        height = Vector3(0,-h,0);
    }
    Vector3 half_width = width*.5;

    //Create points
    //.__.__.
    //._.__._.
    //^points on even + odd rows, w/ c of 3
    for(int i =0; i<r; i++){
        if(i%2==0){ //even
            for(int j = 0; j<c; j++)
                createPoint(start+j*width+i*height);
        }
        else{ //odd
            for(int j = 0; j<=c; j++)
                if(j==0)
                    createPoint(start+i*height);
                else if(j==c)
                    createPoint(start+(j-1)*width+i*height);
                else
                    createPoint(half_width+start+(j-1)*width+i*height);
        }
    }

    //Create structural links
    //|\/\/|       | / /        \ \ |
    //|/\/\|       | \ \        / / |
    //^vertical    ^left        ^right
    for(int i =0; i<r; i++){
        bool even = i%2==0;
        for(int j = 0; (j<c&&even) || (j<=c&&!even); j++){
            int index = j+i*c+i/2;
            if((j<c-1&&even) || (j<c&&!even)) //last point in row doesn't have link after it
                createLink(index, index+1);  //horizontal
            if(i<r-1){ //last row doesn't have link beneath it
                if(even){ //even
                    createLink(index, index+c); //vertical, left
                    createLink(index, index+c+1); //vertical, right
                }
                else{ //odd
                    if(j<c) //furthest right point on odd row doesn't have point to left beneath it
                        createLink(index, index+c+1); //vertical, left
                    if(j>0) //furthest left point on odd row doesn't have point to right beneath it
                        createLink(index, index+c); //vertical, right
                }
            }
        }
    }

    //Triangulate
    for(int i =0; i<r; i++){
        bool even = i%2==0;
        for(int j = 0; (j<c-1&&even) || (j<c&&!even); j++){
            int index = j+i*c+i/2;
            if(i<r-1) //triangles below points of row
                triangulate(index,index+1,index+c+1);
            if(i>0)  //triangles above points of row
                //triangulate(index,index+1,index-c); //alternating normals
                triangulate(index,index-c,index+1);
        }
    }

    //Create bend constraints
    //|     \     /    _._
    //|      \   /
    for(int i = 0; i<r; i++){
        bool even = i%2==0;
        for(int j = 0; j<=c; j++){
            int index = j+i*c+i/2;
            if((j+2<c&&even) || (j+2<=c&&!even))  //horizontal
                createLink(index,index+2);
            if(i+2<r && ((even&&(j==0 || j==c-1)) || (!even&&(j==0 || j==c)))) //vertical, at edges
                createLink(index,index+2*c+1);
            if((even&&j>=1&&j<=c)||(!even&&j>=2&&j<=c)) //diagonal, left
            if(i+2<r && ((even&&j>=1&&j<c)||(!even&&j>=2&&j<c))) //diagonal, left
                createLink(index,index+2*c);
            if(i+2<r && ((even&&j>=0&&j<c-1)||(!even&&j>=1&&j<c-1))) //diagonal, right
                createLink(index,index+2*(c+1));
        }
    }
}

Cloth::~Cloth()
{
    for (std::vector<Tri*>::iterator it = _triangles.begin() ; it != _triangles.end(); ++it)
        delete (*it);
    _triangles.clear();
}

//***************************for update*****************************//
void Cloth::onTick(float ){
    for(unsigned int i = 0; i<_triangles.size(); i++)
        calculate(_triangles.at(i));
}

void Cloth::onDraw(Graphic* g){
    Verlet::onDraw(g);

    g->cull(false);
    for(Tri* t: _triangles){
        g->setColor(Vector4(t->normal.x*.4,.65+t->normal.y*.3,.77+t->normal.z*.15,1));
        g->drawTriangle(t->vertices,t->normal);
    }
    g->cull(true);

    if(visualize)
        drawCorners(g);
}

void Cloth::triangulate(int a, int b, int c){
    Tri* t = new Tri(a,b,c);
    calculate(t);
    _triangles.push_back(t);
}

//***************************for visualization*****************************//
void Cloth::drawCorners(Graphic *g){
    //RBGW
    g->setColor(Vector3(1,0,0));
    g->transform(&Graphic::drawUnitSphere,getPoint(getCorner(0)),0,
                 Vector3(.1,.1,.1));
    g->setColor(Vector3(0,1,0));
    g->transform(&Graphic::drawUnitSphere,getPoint(getCorner(1)),0,
                 Vector3(.1,.1,.1));
    g->setColor(Vector3(0,0,1));
    g->transform(&Graphic::drawUnitSphere,getPoint(getCorner(2)),0,
                 Vector3(.1,.1,.1));
    g->setColor(Vector3(1,1,1));
    g->transform(&Graphic::drawUnitSphere,getPoint(getCorner(3)),0,
                 Vector3(.1,.1,.1));
}

void Cloth::printCorners(){
    int p0 = getCorner(0);
    int p1 = getCorner(1);
    int p2 = getCorner(2);
    int p3 = getCorner(3);

    std::cout<<"Corner 0 (R):"<<p0<<",at:"<<getPoint(p0)<<std::endl;
    std::cout<<"Corner 1 (G):"<<p1<<",at:"<<getPoint(p1)<<std::endl;
    std::cout<<"Corner 2 (B):"<<p2<<",at:"<<getPoint(p2)<<std::endl;
    std::cout<<"Corner 3 (W):"<<p3<<",at:"<<getPoint(p3)<<std::endl;
}

//***************************for access/ control*****************************//
int Cloth::getCorner(int c){
    if(c==0)
        return 0;
    else if(c==1)
       return col-1;
    else if(c==2){
        if(row%2==0)
            return numPoints-col-1;
        else
            return numPoints-col;
    }
    else
        return numPoints-1;
}

void Cloth::pinCorners(){
    this->createPin(getCorner(0));
    this->createPin(getCorner(1));
    this->createPin(getCorner(2));
    this->createPin(getCorner(3));
}


