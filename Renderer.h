#ifndef RENDERER_H
#define RENDERER_H

#include <QVulkanWindow>
#include <vector>
#include <unordered_map>
#include "Camera.h"
#include "Triangle.h"
#include "TriangleSurface.h"
#include "VisualObject.h"
#include "TriangleSurface.h"
#include "wall.h"
#include "player.h"
#include "pickup.h"
#include "npc.h"
#include "door.h"
#include "utilities.h"
#include "VulkanWindow.h"

class Renderer : public QVulkanWindowRenderer
{
public:
    Renderer(QVulkanWindow *w, bool msaa = false);

    //Initializes the Vulkan resources needed,
    // the buffers
    // vertex descriptions for the shaders
    // making the shaders, etc
    void initResources() override;

    //Set up resources - only MVP-matrix for now:
    void initSwapChainResources() override;

    //Empty for now - needed since we implement QVulkanWindowRenderer
    void releaseSwapChainResources() override;

    //Release Vulkan resources when program ends
    //Called by Qt
    void releaseResources() override;

    //Render the next frame
    void startNextFrame() override;

    //Get Vulkan info - just for fun
    void getVulkanHWInfo();

    //return object from vector Pickups?
    VisualObject* checkCollision(VisualObject* v1, VisualObject* v2);

    std::vector<VisualObject*>& getObjects() { return mObjects; }
    std::unordered_map<std::string, VisualObject*>& getMap() { return mMap; }


    void Patrol(float speed, VisualObject* ptr, float min, float max);

protected:

    //Creates the Vulkan shader module from the precompiled shader files in .spv format
    VkShaderModule createShader(const QString &name);

	void setModelMatrix(QMatrix4x4 modelMatrix);

    //The ModelViewProjection MVP matrix
    QMatrix4x4 mProjectionMatrix;
    //Rotation angle of the triangle
    float mRotation{ 0.0f };

    //Vulkan resources:
    QVulkanWindow* mWindow{ nullptr };
    QVulkanDeviceFunctions *mDeviceFunctions{ nullptr };

    VkDeviceMemory mBufferMemory{ VK_NULL_HANDLE };
    VkBuffer mBuffer{ VK_NULL_HANDLE };
 
    VkDescriptorPool mDescriptorPool{ VK_NULL_HANDLE };
    VkDescriptorSetLayout mDescriptorSetLayout{ VK_NULL_HANDLE };
    VkDescriptorSet mDescriptorSet[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT]{ VK_NULL_HANDLE };

    VkPipelineCache mPipelineCache{ VK_NULL_HANDLE };
    VkPipelineLayout mPipelineLayout{ VK_NULL_HANDLE };
    VkPipeline mPipeline{ VK_NULL_HANDLE };
    VkPipelineLayout mPipelineLayout2{ VK_NULL_HANDLE };
    VkPipeline mPipeline2{ VK_NULL_HANDLE };

    VkQueue mGraphicsQueue{ VK_NULL_HANDLE };

    void setRenderPassParameters(VkCommandBuffer commandBuffer);
private:
    friend class VulkanWindow;
    Triangle mTriangle;
    TriangleSurface mSurface;
    VisualObject mVisualObject;
    std::vector<VisualObject*> mObjects;
    std::vector<VisualObject*> mPickups; //i want to have 6 pickups
    std::unordered_map<std::string, VisualObject*> mMap;    // alternativ container

    void createBuffer(VkDevice logicalDevice,
                      const VkDeviceSize uniAlign, VisualObject* visualObject,
                      VkBufferUsageFlags usage=VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    // more flexible buffer creation, the whole point with index buffer is to reduce amount of used vertices and use indeces(ints) instead of floats, which takes more memory
    void createVertexBuffer(const VkDeviceSize uniAlign, VisualObject* visualObject);
    void createIndexBuffer(const VkDeviceSize uniAlign, VisualObject* visualObject);
    BufferHandle createGeneralBuffer(const VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    void DestroyBuffer(BufferHandle handle);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags requiredProperties);


    //VkBuffer& buffer,
    //VkDeviceMemory& bufferMemory) ;
    Camera mCamera;
    Camera insideCamera;
    bool CanSwitch{false};
    bool DoorIsOpen{false};
    bool HouseEntered{false};
    bool CanMove{true};
    bool IsColliding{false};

    class VulkanWindow* mVulkanWindow{ nullptr };

    VkCommandBuffer BeginTransientCommandBuffer();
    void EndTransientCommandBuffer(VkCommandBuffer commandBuffer);





    //----------------------
    //VkDevice logicalDevice;
    //VkPipelineInputAssemblyStateCreateInfo ia;
    //VkGraphicsPipelineCreateInfo pipelineInfo;
};

#endif // RENDERER_H
