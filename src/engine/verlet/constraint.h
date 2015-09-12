#ifndef CONSTRAINT_H
#define CONSTRAINT_H
#include "verlet.h"

class Constraint
{
public:
    Constraint(int i, Axis a, Verlet* verlet, bool s=false);
    virtual ~Constraint();

    virtual void constrain();
    //Visualizes selection/ hovering of constraint's point, if selectable
    virtual void onDraw(Graphic* g);
    virtual void onTick(float seconds);

    Verlet* getVerlet(){return v;}
    int getIndex(){return index;}

    bool getSelect(){return selectable;}
    void setHover(bool h){hovered=h;}
    void setSelect(bool s){selected=s;}

    //void onHover(); //add sound effects later
    //void onSelect();
protected:
    Verlet* v;
    Axis axis;
    int index;

    bool selectable;
    bool selected;
    bool hovered;
};

#endif // CONSTRAINT_H
