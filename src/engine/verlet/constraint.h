#ifndef CONSTRAINT_H
#define CONSTRAINT_H
#include "verlet.h"

//Represents a constraint, and handles how it's solved and visualized
//Note: can use assign() to point axis to another constraint and get values from it

//For implementing subclasses:
    //Control: assign default pointers in constructor for all unspecified axis/es
        //'Unspecified axis/es': those which player can't manipulate- 2 for translation, 1 for rotation, etc
    //currentPos: set in constructor, update in constrain()
    //constrain(): use solveAxis to determine value for all unspecified axis/es (details in .cpp)
class Constraint
{
public:
    Constraint(int i, Axis a, Verlet* verlet, bool s=false);
    virtual ~Constraint();

    //Visualizes selection/ hovering of constraint's point, if selectable
    virtual void onDraw(Graphic* g);
    virtual void onTick(float seconds);

    Verlet* getVerlet(){return v;}
    int getIndex(){return index;}
    Axis getAxis(){return axis;}
    bool getSelect(){return selectable;}
    void setHover(bool h){hovered=h;}
    void setSelect(bool s){selected=s;}
    Vector3 getPos(){return currentPos;}

    //void onHover(); //add sound effects/ other activation effects later
    //void onSelect();

    virtual void constrain();
    //Helper for constrain(): for non-specified axes, enter verlet.index.A and get corrected value
    float solveAxis(Axis a, float orig);

    void assign(Axis a, Constraint* c);  //Axis a's value will be determined by c's corresponding axis
    float& access(Axis a);  //helper for assign

protected:
    Verlet* v;
    Axis axis;
    int index;

    bool selectable;
    bool selected;
    bool hovered;

    Vector3 currentPos; //Stores pos of index, for other constraints to point to
    //Holds pointers (to variables w/in this constraint or others) that determine values for unspecified axes
    float* control [3] = {NULL,NULL,NULL};
    //Whether unspecified axis is pointing to another constraint
    bool cede [3] = {false,false,false};

    //Convenience- for referencing other axes without checking
    int nextA;
    int prevA;
};

#endif // CONSTRAINT_H
