#include "door.h"

door::door() {
    drawType=1;
    Vertex v1{2.0f,   0.0f,  0.0f,   0.3f, 0.2f, 0.7f, 0.0f, 0.0f};
    Vertex v2{2.0f,   2.0f,  0.0f,   0.3f, 0.2f, 0.4f, 0.0f, 0.0f};
    Vertex v3{2.0f,   2.0f,  0.66f,   0.3f, 0.2f, 0.0f, 0.0f, 0.0f};
    Vertex v4{2.0f,   0.0f,  0.66f,   0.3f, 0.2f, 0.2f, 0.0f, 0.0f};

    mVertices.push_back(v1);
    mVertices.push_back(v2);
    mVertices.push_back(v3);
    mVertices.push_back(v3);
    mVertices.push_back(v1);
    mVertices.push_back(v4);

    //center of the door, aka middle point
    Vertex v5{2.0f,0.5f,0.33f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; //height is 0.5, because my player is very low on the ground
    MiddlePoints.push_back(v5);
    radius=1.0f;
    CollisionType=1;
}
