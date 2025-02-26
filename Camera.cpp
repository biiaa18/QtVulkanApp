#include "Camera.h"

Camera::Camera() {}

void Camera::init()
{
    mProjectionMatrix.setToIdentity();
    mViewMatrix.setToIdentity();
}
void Camera::perspective(int degrees, double aspect, double nearplane, double farplane)
{
    mProjectionMatrix.setToIdentity();
    mProjectionMatrix.perspective(degrees, aspect, nearplane, farplane);
    mProjectionMatrix.scale(1.0f, -1.0f, 1.0f);
}

void Camera::lookAt(const QVector3D &eye, const QVector3D &at, const QVector3D &up)
{
    mEye = eye;
    mAt = at;
    mUp = up;
    mViewMatrix.setToIdentity();
    mViewMatrix.lookAt(mEye, mAt, mUp);
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

QMatrix4x4 Camera::cMatrix()
{
    return mProjectionMatrix * mViewMatrix;
}
