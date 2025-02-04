#ifndef VKTRIANGLESURFACE_H
#define VKTRIANGLESURFACE_H
#include <vector>
#include <string>
#include "vertex.h"
#include "visualobject.h"
#include <iostream>
using namespace std;
class VkTriangleSurface: public VisualObject
{
public:
    VkTriangleSurface();
    //vector<Vertex> mVertices;
    vector<Vertex> getVertices() { return mVertices; }
    VkTriangleSurface (const string &filename);
};

#endif // VKTRIANGLESURFACE_H
