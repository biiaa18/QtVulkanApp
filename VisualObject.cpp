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

void VisualObject::setNewPosition(float speed, float new_x,float new_y, float new_z)
{
    // mMatrix.setToIdentity();
    // QVector4D vec4={new_x,new_y,new_z,0.0f};
    // QVector4D x_vec=(vec4*mMatrix.inverted())/speed;   //x=b*A^-1

    // if (mVertices.at(0).x>1.0f || new_z>1.0f){
    //     new_x=1.0f;
    //     new_z=1.0f;
    // }


    // for (float i=0.0;i<2.0;i+=0.1){
    //     mMatrix.translate(x_vec.x() ,x_vec.y(),x_vec.z());
    // }



    // for (int i=0;i<temp->mVertices.size();i++){
    //     mVertices[i].x=new_x;
    //     mVertices[i].y=new_y;
    //     mVertices[i].z=new_z;
    // }

}



void VisualObject::move(float t, float x, float y, float z)
{
    mPosition+=QVector3D(x,y,z)*t;
}

void VisualObject::scale(float s)
{
    //mMatrix.scale(s);
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

QMatrix4x4 VisualObject::getMatrix()
{
    QMatrix4x4 temp;
    temp.translate(mPosition);

    return temp;
}
