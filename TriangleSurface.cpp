#include "TriangleSurface.h"
#include <fstream>
#include <QDebug>
TriangleSurface::TriangleSurface() : VisualObject()
{
    drawType=2;
    Vertex v1{0.0f,   0.0f,  0.0f,   1.0f, 1.0f, 1.0f, 0.0f, 0.0f};
    Vertex v2{0.0f,   0.0f, 10.0f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f};
    Vertex v3{10.0f,   0.0f,  10.0f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f};
    Vertex v4{10.0f,   0.0f,  0.0f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f};


    Vertex v5{5.0f,0.0f,5.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f}; //middle point


    // mVertices.push_back(v1);
    // mVertices.push_back(v2);
    // mVertices.push_back(v3);
    // mVertices.push_back(v3);
    // mVertices.push_back(v1);
    // mVertices.push_back(v4);

    //Pushing 1st triangle,
    mVertices.push_back(v1);
    mVertices.push_back(v2);
    mVertices.push_back(v3);
    mVertices.push_back(v4);

    //Indexes for the two triangles to form a quad
    mIndices.push_back(0);
    mIndices.push_back(1);
    mIndices.push_back(2);
    mIndices.push_back(2);
    mIndices.push_back(3);
    mIndices.push_back(0);

    // mMatrix.scale(0.5f);
   mMatrix.translate(-5.0f, 0, 0); // fra startNextFrame
    //mMatrix.rotate()
}

TriangleSurface::TriangleSurface(const std::string &filename)
{
    std::ifstream inn(filename);
    if (!inn.is_open())
        return;
    // read input from math part of compulsory
    int n;
    Vertex v;
    inn >> n;
    for (auto i=0; i<n; i++)
    {
        inn >> v;
        mVertices.push_back(v);
        //qDebug() << v.x << v.y << v.z;
    }
    inn.close();
}
