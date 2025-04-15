#include "Camera.h"

Camera::Camera() {

    init();
}

void Camera::init()
{
    mProjectionMatrix.setToIdentity();
    mViewMatrix.setToIdentity();
}
void Camera::perspective(int degrees, double aspect, double nearplane, double farplane)
{
    mProjectionMatrix.setToIdentity();
    mProjectionMatrix.perspective(degrees, aspect, nearplane, farplane);
    //mProjectionMatrix.scale(1.0f, -1.0f, 1.0f);
}

void Camera::lookAt(const QVector3D &eye, const QVector3D &at, const QVector3D &up)
{
    mEye = eye;
    mAt = at;
    mUp = up;
    mViewMatrix.setToIdentity();
    mViewMatrix.lookAt(mEye, mAt, mUp);
}

void Camera::pitch(float degrees)
{
    mPitch += degrees;
}

void Camera::yaw(float degrees)
{
    mYaw += degrees;
}

void Camera::update()
{
    //Set ViewMatrix to Identity, then add the new position and rotations
    mViewMatrix.setToIdentity();
    mPosition.setZ(mPosition.z() + mSpeed);
    //mViewMatrix.translate(mPosition);               //Makes rotation work around World Origo
    mViewMatrix.rotate(mYaw, 0.f, 1.f, 0.f);
    mViewMatrix.rotate(mPitch, 1.f, 0.f, 0.f);
    //mViewMatrix.rotate(mYaw, 0.f, 1.f, 0.f);      //pitch then yaw makes camera wonkey
    mViewMatrix.translate(mPosition);             //Makes rotation work around Camera Origo
}

void Camera::setPosition(const QVector3D& position)
{
    mPosition = position;
    update();
}

void Camera::setSpeed(float speed)
{
    mSpeed = speed;
}

void Camera::moveRight(float delta)
{
    mPosition.setX( mPosition.x() + delta);
}
void Camera::moveForward(float delta)
{
    mPosition.setZ( mPosition.z() + delta);
}

void Camera::updateHeigth(float deltaHeigth)
{
    mPosition.setY(mPosition.y() + deltaHeigth);
}

void Camera::translate(float dx, float dy, float dz)
{
    mViewMatrix.translate(dx, dy, dz);
    //Flip projection because of Vulkan's -Y axis
    // mViewMatrix.scale(1.0f, -1.0f, 1.0f);
}

void Camera::rotate(float t, float x, float y, float z)
{
    mViewMatrix.rotate(t,x,y,z);
}

// QMatrix4x4 Camera::cMatrix()
// {
//     return mProjectionMatrix * mViewMatrix;
// }
