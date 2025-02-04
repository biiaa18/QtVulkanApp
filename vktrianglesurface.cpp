#include "vktrianglesurface.h"
#include <fstream>

VkTriangleSurface::VkTriangleSurface() : VisualObject() {
    Vertex v1{0.0f,   0.0f,  0.0f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    Vertex v2{1.0f,   0.0f,  0.0f,   0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    Vertex v3{0.0f,   1.0f,  0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
    Vertex v4{1.0f,   1.0f,  0.0f,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    mVertices.push_back(v1);
    mVertices.push_back(v2);
    mVertices.push_back(v3);
    //new triangle, because "new" 3 vertices
    mVertices.push_back(v3);
    mVertices.push_back(v2);
    mVertices.push_back(v4);

}

VkTriangleSurface::VkTriangleSurface(const string &filename)
{
    ifstream inn(filename);
    if (!inn.is_open())
        return;
    // read input from math part of compulsory


    int n;
    Vertex v;
    inn >> n; // read how many vertices in file
    for (auto i=0; i<n; i++)
    {
        inn >> v;
        mVertices.push_back(v);
        qDebug() << v.x << v.y << v.z;
    }
    //mMatrix.translate(0.0f, 1.0f, 0.0f);
    inn.close();
}

