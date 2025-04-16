#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H
#include "VisualObject.h"
#include "Vertex.h"

class HeightMap: public VisualObject
{
public:
    HeightMap();

    HeightMap(unsigned char* textureData,float grid_size, float step);
    void makeTerrain(unsigned char* textureData, float grid_size, float step);
};

#endif // HEIGHTMAP_H
