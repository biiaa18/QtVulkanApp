#ifndef VISUALOBJECT_H
#define VISUALOBJECT_H

#include <QVulkanWindow>
#include <vector>
#include "vertex.h"
#include "utilities.h"

class VisualObject
{
public:
    std::vector<Vertex> mVertices;
    std::vector<uint32_t> mIndices;  //vector of index for vertex

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
    void setNewPosition(float speed,float new_x,float new_y,float new_z);
    float radius{0.0f};

    int CollisionType{0}; // 0 is for pickups, 1 is for the door, 2 is for entrance
    std::vector<Vertex> NewPositions;

    // inline VkBuffer& getVBuffer() { return mVertexBuffer.mBuffer; };  //vertex buffer
    // inline VkBuffer& getIBuffer() { return mIndexBuffer.mBuffer; }; //index buffer
    // inline VkDeviceMemory& getVBufferMemory() { return mVertexBuffer.mBufferMemory; };
    // inline void setVBuffer(VkBuffer bufferIn) { mVertexBuffer.mBuffer = bufferIn; };
    // inline void setIBuffer(VkBuffer bufferIn) { mIndexBuffer.mBuffer = bufferIn; };
    // inline void setVBufferMemory(VkDeviceMemory bufferMemoryIn) { mVertexBuffer.mBufferMemory= bufferMemoryIn;};
    // inline void setIBufferMemory(VkDeviceMemory bufferMemoryIn) { mIndexBuffer.mBufferMemory= bufferMemoryIn;};

    VkBuffer& getVBuffer() { return mVertexBuffer.mBuffer; };  //vertex buffer
    VkBuffer& getIBuffer() { return mIndexBuffer.mBuffer; }; //index buffer
    VkDeviceMemory& getVBufferMemory() { return mVertexBuffer.mBufferMemory; };
    VkDeviceMemory& getIBufferMemory() { return mIndexBuffer.mBufferMemory; }
    void setVBuffer(VkBuffer bufferIn) { mVertexBuffer.mBuffer = bufferIn; };
    void setIBuffer(VkBuffer bufferIn) { mIndexBuffer.mBuffer = bufferIn; };
    void setVBufferMemory(VkDeviceMemory bufferMemoryIn) { mVertexBuffer.mBufferMemory= bufferMemoryIn;};
    void setIBufferMemory(VkDeviceMemory bufferMemoryIn) { mIndexBuffer.mBufferMemory= bufferMemoryIn;};

    std::vector<Vertex> getVertices() { return mVertices; }
    std::vector<uint32_t> getIndices() { return mIndices; }
    Vertex getVertices(int index) { return mVertices[index]; }
    VisualObject();
    void setName(std::string name);
    std::string getName() const;



    bool MovingRight=true;
protected:
    std::string mName;
    BufferHandle mVertexBuffer;
    BufferHandle mIndexBuffer;
};

#endif // VISUALOBJECT_H

