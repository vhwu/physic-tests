#include "block.h"

Block::Block(char t)
{
    type = t;

    visible[0]=true;visible[1]=true;visible[2]=true;
    visible[3]=true;visible[4]=true;visible[5]=true;
}

Block::~Block()
{

}

