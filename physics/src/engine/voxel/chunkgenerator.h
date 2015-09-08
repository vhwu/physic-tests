#ifndef CHUNKGENERATOR_H
#define CHUNKGENERATOR_H

#include "vector.h"

class ChunkGenerator
{
public:
    ChunkGenerator();
    ~ChunkGenerator();
    virtual void generateChunk(const Vector3& v)=0;
};

#endif // CHUNKGENERATOR_H
