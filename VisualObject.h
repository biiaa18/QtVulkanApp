#ifndef VISUALOBJECT_H
#define VISUALOBJECT_H

#include <QVulkanWindow>
#include <vector>
#include "vertex.h"

class VisualObject
{
public:
    std::vector<Vertex> mVertices;
    std::vector<Vertex> getVertices() { return mVertices; }
    Vertex getVertices(int index) { return mVertices[index]; }
    VisualObject();
    void setName(std::string name);
    std::string getName() const;

    //
    VkDeviceMemory mBufferMemory{ VK_NULL_HANDLE };
    VkBuffer mBuffer{ VK_NULL_HANDLE };
    VkPrimitiveTopology mTopology { VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };
    //
    QMatrix4x4 mMatrix;
    void move(float x, float y, float z);
    void scale(float s);
    void rotate(float t, float x, float y, float z);
    // to render different topology types
    int drawType{0}; // 0 is for fill, 1 is for lines
    void setdrawType(int type){drawType=type;};

    std::vector<Vertex> MiddlePoints;
    Vertex getMiddlePoints(int object_number);
    void updateMiddlePoints(int index,float new_x,float new_y,float new_z);
    float radius{0.0f};

    //Bezier kurve med 3 kontroll punkter c, t er hastighet
    void Patrol(VisualObject* obj,float t,Vertex c0, Vertex c1, Vertex c2);//alt+Enter to generate definition
protected:
    std::string mName;
};

#endif // VISUALOBJECT_H

