#include "vktrianglesurface.h"
#include <fstream>
<<<<<<< Updated upstream:vktrianglesurface.cpp

VkTriangleSurface::VkTriangleSurface() : VisualObject() {
    Vertex v1{0.0f,   0.0f,  0.0f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    Vertex v2{1.0f,   0.0f,  0.0f,   0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    Vertex v3{0.0f,   1.0f,  0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
    Vertex v4{1.0f,   1.0f,  0.0f,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
=======
#include <QDebug>
VkTriangleSurface::VkTriangleSurface() : VisualObject()
{
    Vertex v1{-5.0f,   0.0f,  0.0f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    Vertex v2{0.0f,   0.0f, -5.0f,   0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    Vertex v3{5.0f,   0.0f,  0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
    Vertex v4{0.0f,   0.0f,  5.0f,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    //    Vertex v1{0.0f,   0.0f,  0.0f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    //    Vertex v2{1.0f,   0.0f,  0.0f,   0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    //    Vertex v3{0.0f,   1.0f,  0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
    //    Vertex v4{1.0f,   1.0f,  0.0f,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
>>>>>>> Stashed changes:VkTriangleSurface.cpp
    mVertices.push_back(v1);
    mVertices.push_back(v2);
    mVertices.push_back(v3);
    //new triangle, because "new" 3 vertices
    mVertices.push_back(v3);
    mVertices.push_back(v1);
    mVertices.push_back(v4);

<<<<<<< Updated upstream:vktrianglesurface.cpp
=======
    // mMatrix.scale(0.5f);
    // mMatrix.translate(0.5f, 0, 0); // fra startNextFrame
>>>>>>> Stashed changes:VkTriangleSurface.cpp
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
    mMatrix.translate(0.0f, -9.0f, -5.0f);
    inn.close();
}

