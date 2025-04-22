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



void VisualObject::setPosition(const QVector3D &position)
{
    mPosition = position;
    //update();

}

QVector3D VisualObject::getPosition()
{
    return mPosition;
}

// void Player::updateHeight()
// {
//     mPosition.setY(mPosition.y());
// }

void VisualObject::moveRight(float delta)
{
    mPosition.setX( mPosition.x() + delta);

}

void VisualObject::moveForward(float delta)
{
    mPosition.setZ( mPosition.z() + delta);
}

void VisualObject::setmovingPosition(const QVector3D &position)
{

    mPosition+=position;
    // mPosition.x() += position.x();
    // mPosition.y()=position.y();
    // mPosition.z()=position.z();
}



void VisualObject::move(float t, float x, float y, float z)
{
    mPosition+=QVector3D(x,y,z)*t;
}

void VisualObject::scale(float s)
{
    mPosition=mPosition*s;
}


void VisualObject::rotate(float t, float x, float y, float z)
{
    mMatrix.rotate(t, x, y, z);
    // QMatrix4x4 temp;
    // temp.rotate(mRotation.x(),x, y, z);
    // // temp.rotate(mRotation.y(),1.f, 0.f, 0.f);
    // // temp.rotate(mRotation.z(),1.f, 0.f, 0.f);
}

void VisualObject::setName(std::string name)
{
    mName = name;
}

std::string VisualObject::getName() const
{
    return mName;
}

QMatrix4x4 VisualObject::getMatrix()
{
    QMatrix4x4 temp;
    temp.translate(mPosition);
    //temp.rotate(mRotation.x(),1.f, 0.f, 0.f);

    return temp;
}
