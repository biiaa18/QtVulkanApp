#include "Triangle.h"
#include "VisualObject.h"

Triangle::Triangle() : VisualObject()
{
    drawType=1;

    mVertices.push_back(Vertex{0.0f,  2.0f,  0.0f,   0.2f, 0.1f, 0.0f, 0.0f, 0.0f});
    mVertices.push_back(Vertex{0.0f,   2.0f,  2.0f,   0.2f, 0.1f, 0.0f, 0.0f, 0.0f});
    mVertices.push_back(Vertex{0.0f,   2.99f,  1.0f,   0.2f, 0.1f, 0.0f, 0.0f, 0.0f}); //used pythagoras again to calculate height addition


    // mMatrix.translate(-0.25f, 0, 0); // fra startNextFrame
}
