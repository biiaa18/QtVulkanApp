#ifndef VISUALOBJECT_H
#define VISUALOBJECT_H
#include <QVulkanWindow>
#include <iostream>
#include <vector>
#include "vertex.h"
using namespace std;
class VisualObject
{
public:
    VisualObject();
    vector<Vertex> mVertices;
    vector<Vertex> getVertices() {return mVertices;} //function returns the vector

    VkDeviceMemory mBufferMemory {VK_NULL_HANDLE};
    VkBuffer mBuffer {VK_NULL_HANDLE};
    VkPrimitiveTopology mTopology {VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};

    QMatrix4x4 mMatrix;
    void move(float x, float y=0.0f, float z=0.0f);
    void scale(float s);
    void rotate(float t, float x, float y, float z);

    void setName(string name);
    string getName() const;

protected:
    string mName;
};

#endif // VISUALOBJECT_H
