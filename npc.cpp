#include "npc.h"

NPC::NPC(Vertex start) {
    Vertex v1=start;
    float rate=0.5;
    float new_x=start.x+rate;
    float new_y=start.y+rate;
    float new_z=start.z-rate;
    Vertex v2{start.x,   start.y,  new_z,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    Vertex v3{new_x,   start.y,  new_z,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    Vertex v4{new_x,   start.y,  start.z,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f};


    mVertices.push_back(v1);
    mVertices.push_back(v2);
    mVertices.push_back(v3);
    mVertices.push_back(v3);
    mVertices.push_back(v4);
    mVertices.push_back(v1);

    //centroid
    Vertex v5;
    v5.x=(v1.x+v2.x+v3.x)/3;
    v5.y=(v1.y+v2.y+v3.y)/3;
    v5.z=(v1.z+v2.z+v3.z)/3;
    v5.r=0.0f;
    v5.g=0.0f;
    v5.b=0.0f;
    v5.u=0.0f;
    v5.v=0.0f;
    MiddlePoints.push_back(v5);
    radius=0.28f;
}


