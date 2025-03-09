#include "wall.h"


wall::wall(float blue) {
    Vertex v1{0.0f,   0.0f,  0.0f,   0.3f, 0.2f, blue, 0.0f, 0.0f};
    Vertex v2{0.0f,   2.0f,  0.0f,   0.3f, 0.2f, blue, 0.0f, 0.0f};
    Vertex v3{2.0f,   2.0f,  0.0f,   0.3f, 0.2f, blue, 0.0f, 0.0f};
    Vertex v4{2.0f,   0.0f,  0.0f,   0.3f, 0.2f, blue, 0.0f, 0.0f};
    //first wall
    mVertices.push_back(v1);
    mVertices.push_back(v2);
    mVertices.push_back(v3);
    mVertices.push_back(v3);
    mVertices.push_back(v1);
    mVertices.push_back(v4);

    //center of the house, aka middle point
    Vertex v5{1.0f,1.0f,1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    MiddlePoints.push_back(v5);
    radius=1.0f;
    CollisionType=2;
}
