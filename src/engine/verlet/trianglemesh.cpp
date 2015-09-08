#include "trianglemesh.h"
#include "verletmanager.h"
#include <math.h>
#include <QHash>
#include "shear.h"

//******************************CONSTRUCTING**********************************//
TriangleMesh::TriangleMesh(const Vector2& dimension, float w,
              const Vector3& start, VerletManager* vm, bool s, bool t): Verlet(vm){
    tearable = t;

    int r = dimension.y;
    int c = dimension.x;
    row = r;
    col = c;

    float half_w = w*.5;
    float h = (sqrt(3)/2.0) * w;

    Vector3 width = Vector3(w,0,0);
    Vector3 height = Vector3(0,-h,0);
    Vector3 half_width = Vector3(half_w,0,0);

    //Create points
    for(int i =0; i<r; i++){
        for(int j = 0; j<c; j++){
            if(i%2==0){  //even
                if(j==c-1)
                    createPoint(start+half_width+(j-1)*width+i*height);
                else
                    createPoint(start+j*width+i*height);
            }
            else{  //odd
                if(j==0)
                    createPoint(start+i*height);
                else
                    createPoint(half_width+start+(j-1)*width+i*height);
            }
        }
    }

    //Create structural links
    for(int i =0; i<r; i++){
        for(int j = 0; j<c; j++){
            int index = j+i*c;
            if(j<c-1)  //horizontal
                createLink(index, index+1);
            if(i<r-1)  //vertical
                createLink(index, index+c);
            if(j<c-1&&i<r-1&&i%2==0) //diagonal on even rows
                createLink(index, index+c+1);
            if(j>0&&i<r-1&&i%2==1) //diagonal on odd rows
                createLink(index, index+c-1);
        }
    }

    //Triangulate
    for(int i =0; i<r-1; i++){
        for(int j = 0; j<c; j++){
            if(i%2==0){  //even
                if(j<c-1){
                    //upper triangles
                    int n = c*i+j;
                    triangulate(n,n+c+1,n+c);
                    //lower triangles
                    triangulate(n,n+1,n+c+1);
                }
            }
            else{  //odd
                if(j!=0){
                    int n = c*i+j;
                    //upper triangles
                    triangulate(n,n+c,n+c-1);
                    //lower triangles
                    triangulate(n,n+c-1,n-1);
                }
            }
        }
    }

    //Create shear and bend constraints
    if(s){
        for(int i = 0; i<r; i++){
            for(int j = 0; j<c; j++){
                if(j+2<c)  //horizontal
                    createShear(j+(i*c),(i*c)+j+2,j+1+(i*c));
                if(i+2<r && (j==0 || j==c-1)) //vertical, at edges
                    createShear(j+(i*c),(i+2)*c+j,(i+1)*c+j);
                if(i+2<r && j+1<c){ //diagonal right-down
                    if(j+1!=c-1&& (j!=0||i%2==0)){
                        if(i%2==0)
                            createShear(j+c*i,j+1+c*(i+2),(i+1)*c+j+1);
                        else
                            createShear(j+c*i,j+1+c*(i+2),(i+1)*c+j);
                    }
                    else if(j+1==c-1 && i%2==1)
                        createShear(j+c*i,j+1+c*(i+2),(i+1)*c+j);
                }
                if(i+2<r && j>0 && j-1<c){ //diagonal left-down
                    if((!(j==c-1&&i%2==0)) && (!(j==1&&i%2!=0))){
                        if(i%2==1)
                            createShear(j+c*i,j-1+c*(i+2),(i+1)*c+j-1);
                        else
                            createShear(j+c*i,j-1+c*(i+2),(i+1)*c+j);
                    }
                }
            }
        }
    }

    //set scalar
    for(int i=0; i<numPoints; i++)
        _scalar[i]=1/(float)numTri[i];
}

TriangleMesh::~TriangleMesh()
{
    for (std::vector<Tri*>::iterator it = _triangles.begin() ; it != _triangles.end(); ++it)
        delete (*it);
    _triangles.clear();
}


void TriangleMesh::triangulate(int a, int b, int c){
    Tri* t = new Tri(a,b,c);
    numTri[a]+=1;
    numTri[b]+=1;
    numTri[c]+=1;
    calculate(t);
    _triangles.push_back(t);

    t->edges[0] = findLink(a,b);
    t->edges[1] = findLink(b,c);
    t->edges[2] = findLink(c,a);

    link_to_tri[t->edges[0]]+=t;
    link_to_tri[t->edges[1]]+=t;
    link_to_tri[t->edges[2]]+=t;
}

//******************************EDITING SHEAR**********************************//
Bend* TriangleMesh::createShear(int a, int b, int c, Link* seg1, Link* seg2){
    float length = (_pos[b]-_pos[a]).length();
    Bend* l = new Bend(a, b, length);
    links.push_back(l);

    if(seg1==NULL&&seg2==NULL){
        seg1 = findLink(a,c);
        seg2 = findLink(b,c);
    }

    //Make sure that segments are arranged so that the shared point is between them
    if(seg1->pointB==seg2->pointA){
        l->seg1 = seg1;
        l->seg2 = seg2;
    }
    else if(seg2->pointB==seg1->pointA){
        l->seg2 = seg1;
        l->seg1 = seg2;
    }
    else
        std::cout<<"createShear(): shear constraint's segments are not in linear order"<<std::endl;

    _linkMap[a].shears+=l;
    _linkMap[b].shears+=l;
    _linkMap[c].crosses+=l;
    link_to_shear[seg1]+=l;
    link_to_shear[seg2]+=l;

    return l;
}

void TriangleMesh::removeShear(Bend* s){
    if(s->pointA<0||s->pointA>numPoints||s->pointB<0||s->pointB>numPoints){
        std::cout<<"removeShear(): nonsense value for points"<<std::endl;
        std::cout<<"s:"<<s->pointA<<","<<s->pointB<<std::endl;
        //if(s->seg2==NULL||s->seg1==NULL)
        //    std::cout<<"nulls"<<std::endl;
        std::cout<<"seg2:"<<s->seg2->pointA<<","<<s->seg2->pointB<<std::endl;
        std::cout<<"seg1:"<<s->seg1->pointA<<","<<s->seg1->pointB<<std::endl;
    }
    _linkMap[s->pointA].shears.removeAll(s);
    _linkMap[s->pointB].shears.removeAll(s);
    _linkMap[s->seg1->pointB].crosses.removeAll(s);
    _linkMap[s->seg2->pointA].crosses.removeAll(s); //in case they differ- very necessary
    _linkMap[s->seg1->pointA].crosses.removeAll(s);  //in case they're backwards
    _linkMap[s->seg2->pointB].crosses.removeAll(s);
    link_to_shear[s->seg1].removeAll(s);
    link_to_shear[s->seg2].removeAll(s);

    links.erase(std::remove(links.begin(), links.end(), s), links.end());

    delete s;
}

//********************************TEARING*************************************//
//Returns whether point 'a' needs to be torn
bool TriangleMesh::checkTorn(int a){
    QList<Link*> list = _linkMap[a].links;
    int splitLinks = 0;
    foreach(Link* l, list){
        QList<Tri*> tri = link_to_tri[l];
        if(tri.size()<2) //edge only has 1 triangle -> tearing= necessary
            splitLinks++;
    }
    return splitLinks>2; //called from tearLink, so there's 2 guaranteed
}

//Copies point, and returns index of copy
int TriangleMesh::duplicatePoint(int index){
    _pos[numPoints] = _pos[index];
    _prevPos[numPoints] = _prevPos[index];
    _acc[numPoints] = _acc[index];
    _normal[numPoints] = _normal[index];
    numPoints++;
    return numPoints-1;
}


//Finds all links + triangles adjoining l1 and attached to index
void TriangleMesh::findConnecting(int index, Tri*& t1, Link*& l1, QList<Tri*>& triangles,
                  QList<Link*>& links, QList<int> &points){
    links.push_back(l1);
    triangles.push_back(t1);

    Tri* currentTriangle = t1;
    Link* oppositeEdge = NULL; //other link in triangle connected to new point

    while(currentTriangle!=NULL){
        points.push_back(currentTriangle->a);
        points.push_back(currentTriangle->b);
        points.push_back(currentTriangle->c);

        //find opposite edge
        for(int i = 0; i<3; i++){
            Link* l = currentTriangle->edges[i];
            if((l->pointA==index || l->pointB==index)&&!links.contains(l)){
                oppositeEdge=l;
                links.push_back(oppositeEdge);
            }
        }
        //find the triangle attached to it, set it to currentTriangle
        QList<Tri*> list = link_to_tri[oppositeEdge];
        if(list.size()==2){  //triangle that isn't this one
            currentTriangle = (list[0]==currentTriangle) ? list[1] : list[0];
            triangles.push_back(currentTriangle);
        }
        else
            currentTriangle = NULL;
    }
}

//Replaces l (if it has a triangle on each side) w/ l1 and l2, and reassigns these links
//accordingly to linkMap and shears
void TriangleMesh::tearLink(Link* l){
    QList<Tri*> tri = link_to_tri[l];
    if(tri.size()!=2)
        return;

    //1. Duplicate l into l1 and l2
    //Note: to keep ordering intact for shears, default ordering is set to false
    int a = l->pointA;
    int b = l->pointB;
    Link* l1 = createLink(a,b,false);
    Link* l2 = createLink(a,b,false);

    //2. Reassign 1 to each triangle (t1,t2)
    Tri* t1 = tri[0];
    Tri* t2 = tri[1];
    t1->replaceLink(l,l1);
    t2->replaceLink(l,l2);
    QList<Tri*> test1 = QList<Tri*>();
    test1.push_back(t1);

    QList<Tri*> test2 = QList<Tri*>();
    test2.push_back(t2);
    link_to_tri[l1]=test1;
    link_to_tri[l2]=test2;

    //3. Information about tearing
    bool tearBefore = checkTorn(a);  //whether a will be duplicated
    bool tearAfter = checkTorn(b);   //whether b will be duplicated
    Connected c1_before = findConnecting(a,t1,l1);  //connected to a and l1
    Connected c2_before = findConnecting(a,t2,l2);  //connected to a and l2
    Connected c1_after  = findConnecting(b,t1,l1);  //connected to b and l1
    Connected c2_after  = findConnecting(b,t2,l2);  //connected to b and l2

    //4. Sort shears into 'before' (shares pointB w/ l) or 'after' (shares ptA w/ l)
    QList<Bend*> shears = link_to_shear[l];
    QList<Bend*> before;
    QList<Bend*> after;
    foreach(Bend* s, shears){
        if(s->pointB==l->pointB)
            before+=s;
        else if(s->pointA==l->pointA)
            after+=s;
        else if(s->pointA==l->pointB){
            before+=s;
            std::cout<<"before backwards:"<<s->pointA<<","<<s->pointB;
        }
        else if(s->pointB==l->pointA){
            after+=s;
            std::cout<<"after backwards:"<<s->pointA<<","<<s->pointB;
        }
        else{
            std::cout<<"leftover:"<<std::endl;
            std::cout<<s->pointA<<","<<s->pointB<<std::endl;
        }
    }
    //std::cout<<"accumulative:"<<before.size()+after.size()<<std::endl;
    //std::cout<<"total:"<<shears.size()<<std::endl;

    //5. Assigns l1 and l2 to the segment that was formerly l.
    //If it's a 'single' shear, it's duplicated
    handleShears(tearBefore,before,l,c1_before.connectedLink,c2_before.connectedLink);
    handleShears(tearAfter,after,l,c1_after.connectedLink,c2_after.connectedLink);

    //6. Erase old link
    removeLink(l);


    if(tearBefore){
        int index2 = insertPoint(a,t1,l1,t2,l2,c1_before,c2_before);
        visualizeTear(a,index2,c1_before,c2_before);
    }
    if(tearAfter){
        int index2 = insertPoint(b,t1,l1,t2,l2,c1_after,c2_after);
        visualizeTear(b,index2,c1_after,c2_after);
    }

/*
    //7. Split indices, if necessary
    if(tearBefore){
        int index2 = insertPoint(a,c1_before,c2_before);
        visualizeTear(a,index2,c1_before,c2_before);
    }
    if(tearAfter){
        int index2 = insertPoint(b,c1_after,c2_after);
        visualizeTear(b,index2,c1_after,c2_after);
    }
    if(!tearBefore&&!tearAfter)
        std::cout<<"tearLink(): no new point created"<<std::endl;
    */
}

//OLD***************************************************

//If 'a' needs to be torn, insert another point there is is
//and reassign triangles/ links as necessary
int TriangleMesh::insertPoint(int index, Tri* t1, Link* l1, Tri* t2, Link* l2, Connected c1, Connected c2){
    //1.Duplicate point
    int index2 = duplicatePoint(index);
    //QList<int> points1a = c1.points;
    QList<int> points1a;
    foreach(Tri* t, c1.connectedTri){
        points1a.push_back(t->a);
        points1a.push_back(t->b);
        points1a.push_back(t->c);
    }
    QList<int> points2a;
    foreach(Tri* t, c2.connectedTri){
        points2a.push_back(t->a);
        points2a.push_back(t->b);
        points2a.push_back(t->c);
    }

//    //2.Find triangles + edges on each side (from given edge until next split edge)
//    //Side1: to be reassigned with index2
//    QList<Tri*> connectedTri1; QList<Link*> connectedLink1; QList<int> points1;
//    findConnecting(index,t1,l1,connectedTri1,connectedLink1,points1);
//    //Side2: to keep index
//    QList<Tri*> connectedTri2; QList<Link*> connectedLink2; QList<int> points2;
//    findConnecting(index,t2,l2,connectedTri2,connectedLink2,points2);

    //LOOKUP LINKMAP ONCE!!!

    //3.Assign index2 to triangles (connectedTri1)
    foreach(Tri* t, c1.connectedTri)
        t->replaceIndex(index,index2);
    //points1.removeAll(index);  //update points1 to contain index2 instead of index
    //points1.push_back(index2);
    points1a.push_back(index2);
    points1a.removeAll(index);  //update points1 to contain index2 instead of index


    /*
    std::cout<<"in connected:"<<std::endl;
    foreach(int i, points2a)
        std::cout<<i<<" ";
    std::cout<<std::endl;
    std::cout<<"old method"<<std::endl;
    foreach(int i, points2)
        std::cout<<i<<" ";
    std::cout<<std::endl;
    std::cout<<std::endl;
    */

    //4.Assign index2 to structural edges (connectedLink1): update link_map
    foreach(Link* l, c1.connectedLink){
        (l->pointA==index) ? l->pointA=index2 : l->pointB=index2;
        _linkMap[index2].links+=l;
        _linkMap[index].links.removeOne(l);
    }

    //6.Assign shears their correct indices
    QList<Bend*> shear = _linkMap[index].shears;
    //QList<Bend*> shear_a;  //side 1
    shear_a.clear();
    shear_b.clear();
    //QList<Bend*> shear_b;  //side 2

    foreach(Bend* s, shear){
        if(s->seg1==NULL||s->seg2==NULL)
            std::cout<<"null in shear"<<std::endl;
        else if(s->seg1->pointB!=s->seg2->pointA){
            removeShear(s);
        }
        else{
            foreach(Link* l, c1.connectedLink){
                if(s->seg1==l||s->seg2==l)
                    shear_a.push_back(s);
                else
                    shear_b.push_back(s);
            }
        }
    }
    foreach(Bend* s, shear_a){
        //Assign indices
        (s->pointA==index) ? s->pointA=index2 : s->pointB=index2;
        _linkMap[index2].shears+=s;
        _linkMap[index].shears.removeOne(s);
    }

    //8.Destroy any crossing shear constraints in which a and b aren't on the same side
    QList<Bend*> cross = _linkMap[index].crosses;
    foreach(Bend* c, cross){
        int a = c->pointA;
        int b = c->pointB;
        bool onSide = ((points2a.contains(a)&&points2a.contains(b))||(points1a.contains(a)&&points1a.contains(b)));
        bool onSideOld = ((points2a.contains(a)&&points2a.contains(b))||(points1a.contains(a)&&points1a.contains(b)));
        if(onSide!=onSideOld)
            std::cout<<"not matching???"<<std::endl;
        if(c->seg1==NULL||c->seg2==NULL)
            std::cout<<"null in cross"<<std::endl;


        else if(!onSide) //remove cross map, from link->shear, from bend map
            removeShear(c);
        else if(c->seg1->pointB!=c->seg2->pointA)
            removeShear(c);
        else if(c->seg1->pointB==index2){  //update index
            _linkMap[index2].crosses+=c;
            _linkMap[index].crosses.removeOne(c);
        }

//        int a = c->pointA;
//        int b = c->pointB;
//        bool onSide = ((points2.contains(a)&&points2.contains(b))||(points1.contains(a)&&points1.contains(b)));
//        if(!onSide) //remove cross map, from link->shear, from bend map
//            removeShear(c);



    }


    //recalculate scalar + numTri for index + i (new index)
    numTri[index2] = c1.connectedTri.size();
    _scalar[index2]=1/(float)numTri[index2];

    numTri[index] = numTri[index]-c1.connectedTri.size();
    _scalar[index]=1/(float)numTri[index];

    return index2;
}


//If one of shear b's segments is within 'connected', its l (orig) segment is assigned to 'l1/l2'
bool TriangleMesh::assignLink(QList<Link*> connected, Bend* b, Link* orig){
    Link* l = connected[0];
    Link* bseg = (b->seg1==orig) ? b->seg2 : b->seg1;
    if(connected.contains(bseg)){
        (b->seg1==orig) ? b->seg1=l : b->seg2=l;
        return true;
    }
    return false;
}

//Replaces l in each shear w/ l1/l2, depending on which 'connected links' contains shear's other segment
void TriangleMesh::handleShears(bool tear, QList<Bend*> duplicates, Link* l, QList<Link*> l1_links, QList<Link*> l2_links){
    //The pair of links replacing the original link 'l'
    Link* l1 = l1_links[0];
    Link* l2 = l2_links[0];

    //For 'duplicates' (pairs of shears), l is reassigned
    if(duplicates.size()==2){
        Bend* b1 = duplicates[0];
        Bend* b2 = duplicates[1];
        //l1 and l2 and be assigned arbitrarily as segments to replace l if there's 'tear' (new point formed)
        if(!tear){
            (b1->seg1==l) ? b1->seg1=l1 : b1->seg2=l1;
            (b2->seg1==l) ? b2->seg1=l2 : b2->seg2=l2;
        }
        //If there will be a new point formed, insure each shear in 'duplicates' has valid segments
        else{
            if(!assignLink(l1_links, b1, l)&&!assignLink(l2_links, b1, l))
                std::cout<<"handleDuplicates(): no replacement found for l w/in duplicate shear"<<std::endl;
            if(!assignLink(l1_links, b2, l)&&!assignLink(l2_links, b2, l))
                std::cout<<"handleDuplicates(): no replacement found for l w/in duplicate shear"<<std::endl;
        }
    }
    //For 'singles', the shear is duplicated
    else if(duplicates.size()==1){
        Bend* b = duplicates[0];
        Link* other = (b->seg1==l) ? b->seg2 : b->seg1;
        createShear(b->pointA,b->pointB,b->seg1->pointB,other,l1);
        createShear(b->pointA,b->pointB,b->seg1->pointB,other,l2);
        _linkMap[b->seg1->pointB].crosses.removeOne(b);
        removeShear(b);
    }
}

//Finds all links + triangles adjoining l1 and attached to index
Connected TriangleMesh::findConnecting(int index, Tri*& t1, Link*& l1){
    Connected c;
    c.connectedLink.push_back(l1);
    c.connectedTri.push_back(t1);

    Tri* currentTriangle = t1;
    Link* oppositeEdge = NULL; //other link in triangle connected to index

    while(currentTriangle!=NULL){
        c.points.push_back(currentTriangle->a);
        c.points.push_back(currentTriangle->b);
        c.points.push_back(currentTriangle->c);

        //find opposite edge
        for(int i = 0; i<3; i++){
            Link* l = currentTriangle->edges[i];
            if((l->pointA==index || l->pointB==index)&&!c.connectedLink.contains(l)){
                oppositeEdge=l;
                c.connectedLink.push_back(oppositeEdge);
            }
        }
        //find the triangle attached to it, set it to currentTriangle
        QList<Tri*> list = link_to_tri[oppositeEdge];
        if(list.size()==2){  //triangle that isn't this one
            currentTriangle = (list[0]==currentTriangle) ? list[1] : list[0];
            c.connectedTri.push_back(currentTriangle);
        }
        else
            currentTriangle = NULL;
    }
    return c;
}

//If 'index' needs to be torn, insert another point (index2) and reassign triangles/ links as necessary
//@param c1: contains triangles/links to be reassigned to index2
int TriangleMesh::insertPoint(int index, Connected& c1, Connected& c2){
    //1.Duplicate point
    int index2 = duplicatePoint(index);
    LinkMap& map1 = _linkMap[index];
    LinkMap& map2 = _linkMap[index2];

    //2.Assign index2 to triangles
    foreach(Tri* t, c1.connectedTri)
        t->replaceIndex(index,index2);
    c1.points.removeAll(index);  //update points1 to contain index2 instead of index
    c1.points.push_back(index2);

    //3.Assign index2 to structural edges, and update LinkMap.links
    foreach(Link* l, c1.connectedLink){
        (l->pointA==index) ? l->pointA=index2 : l->pointB=index2;
        map2.links+=l;
        map1.links.removeAll(l);
    }

    //4.Shears: remove invalid shears, assign index2, and upate LinkMap.shears
    QList<Bend*> shear = map1.shears;
    foreach(Bend* s, shear){
        if(s->seg1==NULL||s->seg2==NULL)
            std::cout<<"null in shear"<<std::endl;
        else if(s->seg1->pointB!=s->seg2->pointA){
            //std::cout<<"remove shear"<<std::endl;
            removeShear(s);
        }
        else{
            foreach(Link* l, c1.connectedLink){
                if(s->seg1==l||s->seg2==l){
                    (s->pointA==index) ? s->pointA=index2 : s->pointB=index2;
                    map2.shears+=s;
                    map1.shears.removeAll(s);
                }
            }
        }
    }

    //5.Cross: remove invalid, assign index2, and upate LinkMap.cross
    QList<Bend*> cross = map1.crosses;
    foreach(Bend* c, cross){
        int a = c->pointA;
        int b = c->pointB;
        bool onSide = ((c2.points.contains(a)&&c2.points.contains(b))||(c1.points.contains(a)&&c1.points.contains(b)));
        if(c->seg1==NULL||c->seg2==NULL)
            std::cout<<"null in cross"<<std::endl;
        else if(!onSide){
            //std::cout<<"remove cross"<<std::endl;
            removeShear(c);
        }
        else if(c->seg1->pointB!=c->seg2->pointA){ //necessary?
            //std::cout<<"remove cross x2"<<std::endl;
            removeShear(c);
        }
        else if(c->seg1->pointB==index2){
            map2.crosses+=c;
            map1.crosses.removeAll(c);
        }
    }

    //recalculate scalar + numTri for index + i (new index)
    numTri[index2] = c1.connectedTri.size();
    _scalar[index2]=1/(float)numTri[index2];

    numTri[index] = numTri[index]-c1.connectedTri.size();
    _scalar[index]=1/(float)numTri[index];

    return index2;
}


//********************************UPDATING*************************************//
/*
void TriangleMesh::applyWind(Tri* t){
    //Wind has full effect to perpendicular triangle, none on parallel triangle
    Vector3 windDirection = _manager->wind;
    float windScalar =  windDirection.dot(t->normal);

    if(windScalar<0)
        windScalar*=-1;
    t->windForce = windScalar;

    //Normal-based wind force
    Vector3 windForce = windDirection*windScalar*_manager->windPow;

    //See .h for explanation of modes
    if(windMode == 0){  //normal-based, non-averaged
        _acc[t->a] += windForce;
        _acc[t->b] += windForce;
        _acc[t->c] += windForce;
    }
    else if(windMode == 1){  //normal-based, averaged
        windForce*=2.25;
        _acc[t->a] += windForce*_scalar[t->a];
        _acc[t->b] += windForce*_scalar[t->b];
        _acc[t->c] += windForce*_scalar[t->c];
    }
    else if(windMode ==2){  //no normals, non-averaged
        Vector3 w = windDirection;
        _acc[t->a] += w;
        _acc[t->b] += w;
        _acc[t->c] += w;
    }
    else{  //no normals, averaged
        Vector3 w = windDirection*=2.25;
        _acc[t->a] += w*_scalar[t->a];
        _acc[t->b] += w*_scalar[t->b];
        _acc[t->c] += w*_scalar[t->c];
    }
}
*/
void TriangleMesh::onTick(float ){
        for(unsigned int i = 0; i<_triangles.size(); i++){
            calculate(_triangles.at(i));
            applyWind(_triangles.at(i));
        }
}
//********************************DRAWING*************************************//
void TriangleMesh::visualizeTear(int index, int index2, const Connected& c1, const Connected& c2){
    connected_t1 = c1.connectedTri;
    connected_t2 = c2.connectedTri;
    connected_l1 = c1.connectedLink;
    connected_l2 = c2.connectedLink;
    newLink1 = connected_l1[0];
    newLink2 = connected_l2[0];
    LinkMap map1 = _linkMap[index];
    LinkMap map2 = _linkMap[index2];
    cross_a = map1.crosses;
    cross_b = map2.crosses;
    shear_a = map1.shears;
    shear_b = map2.shears;
}

//visualize triangles + links duplicated point will be replaced in
void TriangleMesh::drawTear(Graphic* g){
    g->depth(false);
    g->cull(false);

    //modes:
    //Connected halves + new links (on by default)
    //Structural color-coded
    //Shear (all)
    //Shear for each half
    //Cross for each half

    //Visualize how many triangles each edge is connected to
    /*
    g->setLineWidth(1);
    for(int i=0; i<links.size(); i++){
        Link* l = links.at(i);
        int n = link_to_tri[l].size();
        if(n==1)
            g->setColor(Vector3(1,0,0)); //red = 1 edge
        else if(n==2)
            g->setColor(Vector3(1,1,0)); //yellow = 2 edges
        else if(n==0)
            g->setColor(Vector4(1,1,1,.2)); //green = ??? edges
        else{
            g->setColor(Vector3(0,1,0));
            std::cout<<n<<std::endl;
        }
        drawLink(l,g);
    }
    */

    //l1 and l2
    g->setLineWidth(3);
    g->setColor(Vector3(1,1,1));
    drawLink(newLink1,g);
    g->setColor(Vector3(.7,.7,.7));
    drawLink(newLink2,g);

    //Connected tri
    g->setColor(Vector4(1,0,0,.5));
    foreach(Tri* t, connected_t1)
        g->drawTriangle(t->vertices,t->normal);
    g->setColor(Vector4(0,1,0,.5));
    foreach(Tri* t, connected_t2)
        g->drawTriangle(t->vertices,t->normal);


    //Shear
    foreach(Bend* b, shear_a){
        g->setColor(Vector3(1,0,0));
        drawBend(b,g);
    }
    foreach(Bend* b, shear_b){
        g->setColor(Vector3(0,0,1));
        drawBend(b,g);
    }


    //Cross
    foreach(Bend* b, cross_a){
        g->setColor(Vector3(1,0,0));
        drawBend(b,g);
    }
    foreach(Bend* b, cross_b){
        g->setColor(Vector3(0,0,1));
        drawBend(b,g);
    }

    g->depth(true);
    g->cull(true);
}

void TriangleMesh::onDraw(Graphic* g){

    g->setColor(Vector3(1,1,1));
    Verlet::onDraw(g);

    g->cull(false);
    for(Tri* t: _triangles){
        g->setColor(Vector4(t->normal.x*.4,.65+t->normal.y*.3,.77+t->normal.z*.15,1));
        g->drawTriangle(t->vertices,t->normal);

        //Normals smooth shading
        //Vector3 n[3]= {_normal[t->a],_normal[t->b],_normal[t->c]};
        //g->drawTriangle(t->vertices,n);
    }
    g->cull(true);

    drawTear(g);

}

void TriangleMesh::drawVertexNormals(Graphic* g){
    for(int i=0; i<numPoints; i++){
        Vector3 center = _pos[i];
        g->drawLine(center, center+_normal[i]);
    }
}


