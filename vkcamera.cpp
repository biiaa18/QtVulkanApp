#include "vkcamera.h"

VkCamera::VkCamera() {}

void VkCamera::init()
{
    mProjectionMatrix.setToIdentity();
    mViewMatrix.setToIdentity();
}
void VkCamera::perspective(int degrees, double aspect, double nearplane, double farplane)
{
    mProjectionMatrix.setToIdentity();
    mProjectionMatrix.perspective(degrees, aspect, nearplane, farplane);
    //Flip projection because of Vulkan's -Y axis
    mProjectionMatrix.scale(1.0f, -1.0f, 1.0f);
}


void VkCamera::lookAt(const QVector3D &eye, const QVector3D &at, const QVector3D &up)
{
    mEye = eye;
    mAt = at;
    mUp = up;
    mViewMatrix.setToIdentity();
    mViewMatrix.lookAt(mEye, mAt, mUp);
}


void VkCamera::translate(float dx, float dy, float dz)
{
    mViewMatrix.translate(dx, dy, dz);
}


QMatrix4x4 VkCamera::cMatrix()
{
    return mProjectionMatrix * mViewMatrix;
}

