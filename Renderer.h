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
#include "objmesh.h"
#include "texture.h"
#include "heightmap.h"

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

    //For Uniform buffers
    VkDescriptorPool mDescriptorPool{ VK_NULL_HANDLE };
    VkDescriptorSetLayout mDescriptorSetLayout{ VK_NULL_HANDLE };
    VkDescriptorSet mDescriptorSet{ VK_NULL_HANDLE }; //[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT]

    VkPipelineCache mPipelineCache{ VK_NULL_HANDLE };
    VkPipelineLayout mPipelineLayout{ VK_NULL_HANDLE };
    VkPipelineLayout mPipelineLayout2{ VK_NULL_HANDLE };
    VkPipeline mPipeline{ VK_NULL_HANDLE };
    VkPipeline mPipeline2{ VK_NULL_HANDLE };

    VkQueue mGraphicsQueue{ VK_NULL_HANDLE };
    void setRenderPassParameters(VkCommandBuffer commandBuffer);

    //For Textures
    VkDescriptorPool mTextureDescriptorPool{ VK_NULL_HANDLE };
    VkDescriptorSetLayout mTextureDescriptorSetLayout{ VK_NULL_HANDLE };
    VkSampler mTextureSampler{ VK_NULL_HANDLE };
    void setTexture(TextureHandle& textureHandle, VkCommandBuffer commandBuffer);



private:
    friend class VulkanWindow;
    Triangle mTriangle;
    TriangleSurface mSurface;
    VisualObject mVisualObject;
    std::vector<VisualObject*> mObjects;
    std::vector<VisualObject*> mPickups; //i want to have 6 pickups
    std::unordered_map<std::string, VisualObject*> mMap;    // alternativ container
    BufferHandle mUniformBuffer{};
    VkSurfaceFormatKHR mSurfaceFormat{};
    TextureHandle mTextureHandle{};
    //For Uniform buffers



    //FUNCTIONS
    //buffers
    void createBuffer(VkDevice logicalDevice,
                      const VkDeviceSize uniAlign, VisualObject* visualObject,
                      VkBufferUsageFlags usage=VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    // more flexible buffer creation, the whole point with index buffer is to reduce amount of used vertices and use indeces(ints) instead of floats, which takes more memory
    void createVertexBuffer(const VkDeviceSize uniformAlignment, VisualObject* visualObject);
    void createIndexBuffer(const VkDeviceSize uniformAlignment, VisualObject *visualObject);
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

    //MORE UNIFORM BUFFERS STUFF
    void createUniformBuffer();
    void createDescriptorSetLayouts();
    void createDescriptorSet();
    void createDescriptorPools();
    void* mUniformBufferLocation{ nullptr };

    //TEXTURES
    //vector<class Texture*> mTextures;  //all textures
    void createTextureSampler();
    TextureHandle createTexture(const char* filename);
    TextureHandle createImage(int width, int height, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkFormat format);
    void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout); //sets up the transfer to the shader
    void copyBufferToImage(VkBuffer buffer, VkImage image, int width, int height);
    VkImageView createImageView(VkImage image, VkFormat format);
    void destroyTexture(TextureHandle& textureHandle);





    // Color shader material / shader
    struct {
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
        //VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };    //also should have had a spesific pipeline layout
        VkPipeline pipeline{ VK_NULL_HANDLE };
    } mColorMaterial;


    //----------------------
    //VkDevice logicalDevice;
    //VkPipelineInputAssemblyStateCreateInfo ia;
    //VkGraphicsPipelineCreateInfo pipelineInfo;
};

#endif // RENDERER_H
