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

void VisualObject::updateMiddlePoints(int index,float new_x,float new_y,float new_z){ //index is technically 0, because object has 1 middle point,but it can be later reused on house with several "walls"
    MiddlePoints[index].x+=new_x;
    MiddlePoints[index].y+=new_y;
    MiddlePoints[index].z+=new_z;

}

void VisualObject::setNewPosition(VisualObject* ptr,float new_x, float new_z)
{
    VisualObject* temp=ptr;
    for (int i=0;i<temp->mVertices.size();i++){
        mVertices[i].x=new_x;
        mVertices[i].z=new_z;
    }
};


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
