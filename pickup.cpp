#include "pickup.h"

Pickup::Pickup() {
    Vertex v1{-3.0f,  0.1f,  5.0f,   0.5f, 0.5f, 0.5f, 0.0f, 0.0f};
    Vertex v2{-3.25f,   0.5f,  5.0f,   0.5f, 0.5f, 0.5f, 0.0f, 0.0f};
    Vertex v3{-3.5f,   0.1f,  5.0f,   0.5f, 0.5f, 0.5f, 0.0f, 0.0f};
    mVertices.push_back(v1);
    mVertices.push_back(v2);
    mVertices.push_back(v3);


    Vertex v4;
    v4.x=(v1.x+v2.x+v3.x)/3;
    v4.y=(v1.y+v2.y+v3.y)/3;
    v4.z=(v1.z+v2.z+v3.z)/3;
    v4.r=0.0f;
    v4.g=0.0f;
    v4.b=0.0f;
    v4.u=0.0f;
    v4.v=0.0f;
    MiddlePoints.push_back(v4);
    radius=0.28f;
    CollisionType=0;
}
