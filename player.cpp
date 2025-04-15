#include "player.h"

Player::Player() {            //right  up   forward
    drawType=1;
    Vertex v1{0.0f,  0.1f,  8.0f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    Vertex v2{0.5f,   0.1f,  8.0f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    Vertex v3{0.25f,   0.1f,  7.75f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
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
}
