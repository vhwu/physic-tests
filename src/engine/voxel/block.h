#ifndef BLOCK_H
#define BLOCK_H

#pragma pack(push,1)
struct Block
{
public:
    Block(char t);
    ~Block();

    char type;
    //whether to render each face
    bool visible[6];
};
#pragma pack(pop)

#endif // BLOCK_H
