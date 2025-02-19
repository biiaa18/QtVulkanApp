#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QVulkanWindow>
#include <vector>
#include "VkTriangle.h"
#include "VkTrianglesurface.h"
#include "visualobject.h"
#include "vkcamera.h"
using namespace std;

class RenderWindow : public QVulkanWindowRenderer
{
public:
    RenderWindow(QVulkanWindow *w, bool msaa = false);
    RenderWindow(); //new constructor for vulkan window

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

    vector<VisualObject*>& getObjects() { return mObjects; }
    unordered_map<string, VisualObject*>&getMap(){return mMap;};

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

private:
    friend class VulkanWindow; //vulkan window can now access private objects of render window class, so we can rotate/scale/etc using key operations in vulkan window
    VkTriangle mTriangle;
    VkTriangleSurface mSurface;
    VisualObject mVisualObject;
    vector<VisualObject*> mObjects;
    unordered_map<string, VisualObject*> mMap;


    void createBuffer(VkDevice logicalDevice,
                        const VkDeviceSize uniAlign, VisualObject* visualObject,
                        VkBufferUsageFlags usage=VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
                        //VkBuffer& buffer,
                        //VkDeviceMemory& bufferMemory) ;
    VkCamera mCamera;
    //VkDevice logicalDevice;
    //VkPipelineInputAssemblyStateCreateInfo ia;
    //VkGraphicsPipelineCreateInfo pipelineInfo;



};

#endif // RENDERWINDOW_H
