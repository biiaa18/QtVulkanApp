#include "VisualObject.h"

VisualObject::VisualObject()
{
    //Vertex v1{-1.0f,   0.0f,  0.0f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    //Vertex v2{1.0f,   0.0f,  0.0f,   0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    //Vertex v3{0.0f,   1.0f,  0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
    //Vertex v4{1.0f,   1.0f,  0.0f,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    //mVertices.push_back(v1);
    //mVertices.push_back(v2);
    //mVertices.push_back(v3);

    //mVertices.push_back(v3);
    //mVertices.push_back(v2);
    //mVertices.push_back(v4);
    mMatrix.setToIdentity();
}

Vertex VisualObject::getMiddlePoints(int object_number){
    int index=object_number;

    return MiddlePoints[index];
};

void VisualObject::updateMiddlePoints(int index,float new_x,float new_y,float new_z){
    MiddlePoints[index].x+=new_x;
    MiddlePoints[index].y+=new_y;
    MiddlePoints[index].z+=new_z;

};

void VisualObject::Patrol(VisualObject* obj, float t,Vertex c0, Vertex c1, Vertex c2)
{
    // float x=....t ...c;
    // float y=....t ...c;
    //move(x,y,0.0f);
}

void VisualObject::move(float x, float y, float z)
{
    mMatrix.translate(x, y, z);
}

void VisualObject::scale(float s)
{
    mMatrix.scale(s);
}


void VisualObject::rotate(float t, float x, float y, float z)
{
    mMatrix.rotate(t, x, y, z);
}

void VisualObject::setName(std::string name)
{
    mName = name;
}

std::string VisualObject::getName() const
{
    return mName;
}
