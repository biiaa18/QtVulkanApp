#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H
#include "VisualObject.h"

class HeightMap: public VisualObject
{
public:
    HeightMap();

    void makeTerrain(unsigned char* textureData, float grid_size, float step);
};

#endif // HEIGHTMAP_H
