 #include "Renderer.h"
#include <QVulkanFunctions>
#include <QFile>
#include <QtMath>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;



/*** Renderer class ***/

Renderer::Renderer(QVulkanWindow *w, bool msaa)
	: mWindow(w)
{
    if (msaa) {
        const QList<int> counts = w->supportedSampleCounts();
        qDebug() << "Supported sample counts:" << counts;
        for (int s = 16; s >= 4; s /= 2) {
            if (counts.contains(s)) {
                qDebug("Requesting sample count %d", s);
                mWindow->setSampleCount(s);
                break;
            }
        }
    }



    //plane
    mObjects.push_back((new TriangleSurface()));  //0

    // //player
    mObjects.push_back((new Player()));//1

    // // house: i decided to do walls this way, was less lines, so i didnt push vertices for each wall in house.cpp
    // //walls
    mObjects.push_back((new wall(0.0))); //2
    mObjects.push_back((new wall(0.0))); //3
    mObjects.at(3)->move (0.0f,0.0f,2.0f);
    mObjects.push_back((new wall(0.0))); //4
    mObjects.at(4)->rotate(-90.0f, 0.0f, 1.0f, 0.0f);
    mObjects.push_back((new door())); //5   is the actual door
    mObjects.at(5)->move (0.0f,0.0f,0.67f);
    mObjects.at(5)->updateMiddlePoints(0,0.0f,0.0f,0.66f);
    mObjects.push_back((new door())); //6
    mObjects.push_back((new door())); //7
    mObjects.at(7)->move (0.0f,0.0f,1.35f);
    //roof: used pythagoras here to understand the translation
    mObjects.push_back((new wall(1.0))); //8
    mObjects.at(8)->move (0.0f,1.59f,-0.41f);
    mObjects.at(8)->rotate(45.0f, 1.0f, 0.0f, 0.0f);
    mObjects.push_back((new wall(1.0))); //9
    mObjects.at(9)->move (0.0f,1.59f,2.41f);
    mObjects.at(9)->rotate(-45.0f, 1.0f, 0.0f, 0.0f);
    mObjects.push_back((new Triangle())); //10
    mObjects.push_back((new Triangle())); //11
    mObjects.at(11)->move (2.0f,0.0f,0.0f);

    // //Axis
    // mObjects.push_back((new TriangleSurface("D:\\1x_axis.txt")));
    // mObjects.at(10)->setdrawType(1);
    // mObjects.push_back((new TriangleSurface("D:\\1y_axis.txt")));
    // mObjects.at(11)->setdrawType(1);
    // mObjects.push_back((new TriangleSurface("D:\\1z_axis.txt")));
    // mObjects.at(12)->setdrawType(1);

    // //Pickups
    mPickups.push_back((new Pickup()));//0
    mPickups.push_back((new Pickup()));//1
    mPickups.at(1)->move (0.0f,0.0f,-2.0f);
    mPickups.at(1)->updateMiddlePoints(0,0.0f,0.0f,-2.0f);
    mPickups.push_back((new Pickup()));//2
    mPickups.at(2)->move (0.0f,0.0f,3.0f);
    mPickups.at(2)->updateMiddlePoints(0,0.0f,0.0f,3.0f);
    mPickups.push_back((new Pickup()));//3
    mPickups.at(3)->move (5.0f,0.0f,0.0f);
    mPickups.at(3)->updateMiddlePoints(0,5.0f,0.0f,0.0f);
    mPickups.push_back((new Pickup()));//4
    mPickups.at(4)->move (4.0f,0.0f,3.0f);
    mPickups.at(4)->updateMiddlePoints(0,4.0f,0.0f,3.0f);
    mPickups.push_back((new Pickup()));//5
    mPickups.at(5)->move (4.0f,0.0f,-4.0f);
    mPickups.at(5)->updateMiddlePoints(0,4.0f,0.0f,-4.0f);

    // //NPC
    mObjects.push_back((new NPC(mPickups.at(0)->getMiddlePoints(0)))); //must always be under pickups creation....
    mObjects.push_back((new NPC(mPickups.at(2)->getMiddlePoints(0))));


    //OBJECT
    mObjects.push_back((new ObjMesh("sphere.obj")));

    mObjects.at(0)->setName("plane");
    // mObjects.at(1)->setName("wall1");
    // mObjects.at(2)->setName("wall2");
    // **************************************
    // Legger inn objekter i map
    // **************************************
    //string navn{"navn"};
    for (auto it=mObjects.begin(); it!=mObjects.end(); it++){
        mMap.insert(pair<string, VisualObject*>{(*it)->getName(),*it});
    }

    mVulkanWindow = dynamic_cast<VulkanWindow*>(w);
}

void Renderer::initResources()
{
    qDebug("\n ***************************** initResources ******************************************* \n");

    VkDevice logicalDevice = mWindow->device();
    mDeviceFunctions = mWindow->vulkanInstance()->deviceFunctions(logicalDevice);



    // Initialize the graphics queue
    uint32_t graphicsQueueFamilyIndex = mWindow->graphicsQueueFamilyIndex();
    mDeviceFunctions->vkGetDeviceQueue(logicalDevice, graphicsQueueFamilyIndex, 0, &mGraphicsQueue);


    //const int concurrentFrameCount = mWindow->concurrentFrameCount(); // 2 on Oles Machine
    const VkPhysicalDeviceLimits *pdevLimits = &mWindow->physicalDeviceProperties()->limits;
    const VkDeviceSize uniAlign = pdevLimits->minUniformBufferOffsetAlignment;
    qDebug("uniform buffer offset alignment is %u", (uint)uniAlign); //64 on Oles machine

    /// Dag 240125:
    // VkBufferCreateInfo bufferInfo{};
    // memset(&bufferInfo, 0, sizeof(bufferInfo)); //Clear out the memory
    // bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; // Set the structure type

    for (auto it=mObjects.begin(); it!=mObjects.end(); it++)
    {
        createVertexBuffer(uniAlign, *it);
        //createBuffer(logicalDevice, uniAlign, *it);

        if ((*it)->getIndices().size() > 0) {//If object has indices
            createIndexBuffer(uniAlign, *it);
        }


    }

    for (auto it=mPickups.begin(); it!=mPickups.end(); it++)
    {
        createVertexBuffer(uniAlign, *it);
        //createBuffer(logicalDevice, uniAlign, *it);

        if ((*it)->getIndices().size() > 0) {//If object has indices
            createIndexBuffer(uniAlign, *it);

        }

    }

    //DescriptorSets must be made before the Pipelines
    createDescriptorSetLayouts();

    /********************************* Vertex layout: *********************************/
    VkVertexInputBindingDescription vertexBindingDesc ={};
    vertexBindingDesc.binding = 0;
    vertexBindingDesc.stride = sizeof(Vertex);
    vertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    /********************************* Shader bindings: *********************************/
    //Descritpion of the attributes used for vertices in the shader
    VkVertexInputAttributeDescription vertexAttrDesc[3];    //Updated to a more common way to write it
    vertexAttrDesc[0].location = 0; //position
    vertexAttrDesc[0].binding = 0;
    vertexAttrDesc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexAttrDesc[0].offset = 0;

    vertexAttrDesc[1].location = 1; //color or normal
    vertexAttrDesc[1].binding = 0;
    vertexAttrDesc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexAttrDesc[1].offset = 3 * sizeof(float);           // could use offsetof(Vertex, r); from <cstddef>

    vertexAttrDesc[2].location = 2;	    //UV
    vertexAttrDesc[2].binding = 0;
    vertexAttrDesc[2].format = VK_FORMAT_R32G32_SFLOAT;
    vertexAttrDesc[2].offset = 6 * sizeof(float);           // 6 floats before the UVs are found


    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = nullptr;
    vertexInputInfo.flags = 0;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDesc;
    vertexInputInfo.vertexAttributeDescriptionCount = sizeof(vertexAttrDesc) / sizeof(vertexAttrDesc[0]);   // will be 3
    vertexInputInfo.pVertexAttributeDescriptions = vertexAttrDesc;

    // Pipeline cache - supposed to increase performance
    VkPipelineCacheCreateInfo pipelineCacheInfo{};
    pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    VkResult result = mDeviceFunctions->vkCreatePipelineCache(logicalDevice, &pipelineCacheInfo, nullptr, &mPipelineCache);
    if (result != VK_SUCCESS)
        qFatal("Failed to create pipeline cache: %d", result);

    // Pipeline layout
    // PushConstants update - set up the push constant info
    VkPushConstantRange pushConstantRange{};                //Updated to more common way to write it
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = 16 * sizeof(float);            // 16 floats for the model matrix


    //DESCRIPTOR
    array<VkDescriptorSetLayout, 2> descriptorSetLayouts = { mDescriptorSetLayout, mTextureDescriptorSetLayout };

    //LAYOUT COUNT

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    result = mDeviceFunctions->vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout);
    if (result != VK_SUCCESS)
        qFatal("Failed to create pipeline layout: %d", result);

    /// previous version:
    // VkPipelineLayoutCreateInfo pipelineLayoutInfo;
    // memset(&pipelineLayoutInfo, 0, sizeof(pipelineLayoutInfo));
    // pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    // pipelineLayoutInfo.setLayoutCount = 2;
    // pipelineLayoutInfo.pSetLayouts = &descriptorSetLayouts[0];
    // pipelineLayoutInfo.pushConstantRangeCount = 1;  // OEF: PushConstants update
    // pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // OEF: PushConstants update
    // result = mDeviceFunctions->vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout);
    // if (result != VK_SUCCESS)
    //     qFatal("Failed to create pipeline layout: %d", result);




    /********************************* Create shaders *********************************/
     //TEXTURE SHADERS
    VkShaderModule vertShaderModule = createShader(QStringLiteral(":/texture_vert.spv"));
    VkShaderModule fragShaderModule = createShader(QStringLiteral(":/texture_frag.spv"));

    //Updated to more common way to write it:
    VkPipelineShaderStageCreateInfo vertShaderCreateInfoT{};
    vertShaderCreateInfoT.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderCreateInfoT.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderCreateInfoT.module = vertShaderModule;
    vertShaderCreateInfoT.pName = "main";                // start function in shader

    VkPipelineShaderStageCreateInfo fragShaderCreateInfoT{};
    fragShaderCreateInfoT.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderCreateInfoT.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderCreateInfoT.module = fragShaderModule;
    fragShaderCreateInfoT.pName = "main";                // start function in shader

    VkPipelineShaderStageCreateInfo shaderStagesT[] = { vertShaderCreateInfoT, fragShaderCreateInfoT };

    //COLOR SHADERS

    mColorMaterial.vertShaderModule = createShader(QStringLiteral(":/color_vert.spv"));
    mColorMaterial.fragShaderModule = createShader(QStringLiteral(":/color_frag.spv"));

    //Updated to more common way to write it:
    VkPipelineShaderStageCreateInfo vertShaderCreateInfoC{};
    vertShaderCreateInfoC.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderCreateInfoC.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderCreateInfoC.module = mColorMaterial.vertShaderModule;
    vertShaderCreateInfoC.pName = "main";                // start function in shader

    VkPipelineShaderStageCreateInfo fragShaderCreateInfoC{};
    fragShaderCreateInfoC.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderCreateInfoC.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderCreateInfoC.module = mColorMaterial.fragShaderModule;
    fragShaderCreateInfoC.pName = "main";                // start function in shader

    VkPipelineShaderStageCreateInfo shaderStagesC[] = { vertShaderCreateInfoC, fragShaderCreateInfoC };



    /*******************************  Graphics pipeline   **********************************************/

    VkGraphicsPipelineCreateInfo pipelineInfo{};    //Will use this variable a lot in the next 100s of lines
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2; //vertex and fragment shader
    pipelineInfo.pStages = shaderStagesT;
    pipelineInfo.pVertexInputState = &vertexInputInfo;


    // **** The viewport and scissor will be set dynamically via vkCmdSetViewport/Scissor in setRenderPassParameters().
    // This way the pipeline does not need to be touched when resizing the window.
    VkPipelineViewportStateCreateInfo viewport{};
    viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport.viewportCount = 1;
    viewport.scissorCount = 1;
    pipelineInfo.pViewportState = &viewport;

    // **** Input Assembly **** - describes how primitives are assembled in the Graphics pipeline
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;       //Draw triangles
    inputAssembly.primitiveRestartEnable = VK_FALSE;                    //Allow strips to be connected, not used in TriangleList
    pipelineInfo.pInputAssemblyState = &inputAssembly;

    // **** Rasterizer **** - takes the geometry and turns it into fragments
    VkPipelineRasterizationStateCreateInfo rasterization{};
    rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization.polygonMode = VK_POLYGON_MODE_FILL;           // VK_POLYGON_MODE_LINE will make a wireframe;
    rasterization.cullMode = VK_CULL_MODE_NONE;                 // VK_CULL_MODE_BACK_BIT will cull backsides
    rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;  // Front face is counter clockwise - could be clockwise with VK_FRONT_FACE_CLOCKWISE
    rasterization.lineWidth = 1.0f;                             // Not important for VK_POLYGON_MODE_FILL
    pipelineInfo.pRasterizationState = &rasterization;

    // Enable multisampling
    VkPipelineMultisampleStateCreateInfo multisample{};
    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.rasterizationSamples = mWindow->sampleCountFlagBits();
    pipelineInfo.pMultisampleState = &multisample;

    // **** Color Blending **** -
    // how to blend the color of a fragment that is already in the framebuffer with the color of the fragment being added

    VkPipelineColorBlendAttachmentState colorBlendAttachment{}; // Need this struct for ColorBlending CreateInfo
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
                                          | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;  // Colors to apply blending to - was hardcoded to 0xF;

    VkPipelineColorBlendStateCreateInfo colorBlend{};
    colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlend.attachmentCount = 1;                             // the one we made above
    colorBlend.pAttachments = &colorBlendAttachment;
    pipelineInfo.pColorBlendState = &colorBlend;                // no blending for now, write out all of rgba

    // **** Depth Stencil ****
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    pipelineInfo.pDepthStencilState = &depthStencil;


    // **** Dynamic State **** - dynamic states can be changed without recreating the pipeline
    VkDynamicState dynamicEnable[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamic{};
    dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic.dynamicStateCount = sizeof(dynamicEnable) / sizeof(VkDynamicState);
    dynamic.pDynamicStates = dynamicEnable;
    pipelineInfo.pDynamicState = &dynamic;

    pipelineInfo.layout = mPipelineLayout;
    pipelineInfo.renderPass = mWindow->defaultRenderPass();

    result = mDeviceFunctions->vkCreateGraphicsPipelines(logicalDevice, mPipelineCache, 1, &pipelineInfo, nullptr, &mPipeline);
    if (result != VK_SUCCESS)
        qFatal("Failed to create graphics pipeline: %d", result);

    //Making a pipeline for drawing lines
    mColorMaterial.pipeline = mPipeline;                       // reusing most of the settings from the first pipeline
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;   // draw lines
    rasterization.polygonMode = VK_POLYGON_MODE_FILL;           // VK_POLYGON_MODE_LINE will make a wireframe; VK_POLYGON_MODE_FILL
    rasterization.lineWidth = 5.0f;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pStages = shaderStagesC;
    result = mDeviceFunctions->vkCreateGraphicsPipelines(logicalDevice, mPipelineCache, 1, &pipelineInfo, nullptr, &mColorMaterial.pipeline);
    if (result != VK_SUCCESS)
        qFatal("Failed to create graphics pipeline: %d", result);


    // Destroying the shader modules, we won't need them anymore after the pipeline is created
    if (vertShaderModule)
        mDeviceFunctions->vkDestroyShaderModule(logicalDevice, vertShaderModule, nullptr);
    if (fragShaderModule)
        mDeviceFunctions->vkDestroyShaderModule(logicalDevice, fragShaderModule, nullptr);
    if (mColorMaterial.vertShaderModule)
        mDeviceFunctions->vkDestroyShaderModule(logicalDevice, mColorMaterial.vertShaderModule, nullptr);
    if (mColorMaterial.fragShaderModule)
        mDeviceFunctions->vkDestroyShaderModule(logicalDevice, mColorMaterial.fragShaderModule, nullptr);


    // Create the uniform buffer
    createUniformBuffer();
    createDescriptorPools();
    createDescriptorSet();


    //*************************************************TEXTURE***************************
    // Create the texture sampler
    createTextureSampler();

    //TEXTURE
    mTextureHandle = createTexture("hund.bmp");


    //************************************************* HEIGHT  MAP   ***************************
    //HEIGHT MAP
    //mTextureHandle = createTexture("D:\\HeightMap\\Heightmap.jpg");  // "D:\HeightMap\Heightmap.jpg"


    getVulkanHWInfo(); // if you want to get info about the Vulkan hardware
}


// This function is called at startup and when the app window is resized
void Renderer::initSwapChainResources()
{
    qDebug("\n ***************************** initSwapChainResources ******************************************* \n");

    // Projection matrix - how the scene will be projected into the render window

    //This function is called at startup and when the app window is resized
    mProjectionMatrix.setToIdentity();
    //find the size of the window
    const QSize sz = mWindow->swapChainImageSize();

    mCamera.perspective(45.0f, sz.width() / (float) sz.height(), 0.01f, 100.0f);
    mCamera.translate(0, 2, -15); //Camera is -15 away from origo
    mCamera.rotate(30,1.0f,0.0f,0.0f);

    //                                     if camera goes closer than 0.01f, it starts "clipping" trhough the walls
    insideCamera.perspective(50.0f, sz.width() / (float) sz.height(), 0.01f, 100.0f); //first parameter brings camera closer to the scene, last one gives perspective=how far can you see with camera)
    insideCamera.translate(-1.0, 1, -2.6); // -right, +backwards,- up
    insideCamera.rotate(70,1.0f,0.0f,0.0f); // we look at the scene from the top

}

void Renderer::setRenderPassParameters(VkCommandBuffer commandBuffer)
{
    const QSize swapChainImageSize = mWindow->swapChainImageSize();

    //Backtgound color of the render window - dark grey
    VkClearColorValue clearColor = { { 0.3, 0.3, 0.3, 1 } };

    VkClearDepthStencilValue clearDepthStencil = { 1, 0 };
    VkClearValue clearValues[3]{};  //C++11 {} works even on arrays!
    clearValues[0].color = clearValues[2].color = clearColor;
    clearValues[1].depthStencil = clearDepthStencil;

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = mWindow->defaultRenderPass();
    renderPassBeginInfo.framebuffer = mWindow->currentFramebuffer();
    renderPassBeginInfo.renderArea.extent.width = swapChainImageSize.width();
    renderPassBeginInfo.renderArea.extent.height = swapChainImageSize.height();
    renderPassBeginInfo.clearValueCount = mWindow->sampleCountFlagBits() > VK_SAMPLE_COUNT_1_BIT ? 3 : 2;
    renderPassBeginInfo.pClearValues = clearValues;
    mDeviceFunctions->vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    //Viewport - area of the image to render to, usually (0,0) to (width, height)
    VkViewport viewport{};
    viewport.x = viewport.y = 0.f;
    viewport.width = swapChainImageSize.width();
    viewport.height = swapChainImageSize.height();
    viewport.minDepth = 0.f;                //min framebuffer depth
    viewport.maxDepth = 1.f;                //max framebuffer depth
    mDeviceFunctions->vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    //Scissor - area to draw in the target frame buffer
    VkRect2D scissor{};
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = viewport.width;
    scissor.extent.height = viewport.height;
    mDeviceFunctions->vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}


void Renderer::startNextFrame()
{
    //mVulkanWindow->handleInput();
    //mCamera.update();

    VkCommandBuffer cmdBuf = mWindow->currentCommandBuffer();
    setRenderPassParameters(cmdBuf);

    VkDeviceSize vbOffset{ 0 };     //Offsets into buffer being bound

    //DESCRIPTOR BINDING
    mDeviceFunctions->vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,&mDescriptorSet, 0, nullptr);

    setViewProjectionMatrix();

    /********************************* Our draw call!: *********************************/

    for (auto it=mObjects.begin(); it!=mObjects.end(); it++)
    {
        if ((*it)->drawType==0){
            //pipeline 2 for triangle list
            mDeviceFunctions->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);


        }
        else{
            //pipeline1 for line list
            mDeviceFunctions->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mColorMaterial.pipeline);
        }
        // if camera can switch, we switch to insideCamera from mMatrix
        if(CanSwitch==false){
            //TEXTURE
            //setTexture(mTextureHandle, cmdBuf);

            //BUFFERS
            mDeviceFunctions->vkCmdBindVertexBuffers(cmdBuf, 0, 1, &(*it)->getVBuffer(), &vbOffset);
            setModelMatrix(mCamera.cMatrix() * (*it)->mMatrix);
            //mDeviceFunctions->vkCmdDraw(cmdBuf, (*it)->mVertices.size(), 1, 0, 0);
            if ((*it)->getIndices().size() > 0)
            {
                mDeviceFunctions->vkCmdBindIndexBuffer(cmdBuf, (*it)->getIBuffer(), 0, VK_INDEX_TYPE_UINT32);
                mDeviceFunctions->vkCmdDrawIndexed(cmdBuf, (*it)->getIndices().size(), 1, 0, 0, 0); //size == number of indices
            }
            else {  //No index buffer - use regular draw
                mDeviceFunctions->vkCmdDraw(cmdBuf, (*it)->getVertices().size(), 1, 0, 0);

            }
        }
        else{
            //TEXTURE
            //setTexture(mTextureHandle, cmdBuf);

            //BUFFERS
            mDeviceFunctions->vkCmdBindVertexBuffers(cmdBuf, 0, 1, &(*it)->getVBuffer(), &vbOffset);
            setModelMatrix(insideCamera.cMatrix() * (*it)->mMatrix);
            //mDeviceFunctions->vkCmdDraw(cmdBuf, (*it)->mVertices.size(), 1, 0, 0);
            if ((*it)->getIndices().size() > 0)
            {
                mDeviceFunctions->vkCmdBindIndexBuffer(cmdBuf, (*it)->getIBuffer(), 0, VK_INDEX_TYPE_UINT32);
                mDeviceFunctions->vkCmdDrawIndexed(cmdBuf, (*it)->getIndices().size(), 1, 0, 0, 0); //size == number of indices
            }
            else {  //No index buffer - use regular draw
                mDeviceFunctions->vkCmdDraw(cmdBuf, (*it)->getVertices().size(), 1, 0, 0);

            }
        }


    }


    // //for rendering pickups
    for (auto it=mPickups.begin(); it!=mPickups.end(); it++){
        if ((*it)->drawType==0){
            //pipeline 1 for triangle list
            mDeviceFunctions->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
            //mDeviceFunctions->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mTexturePipeline);

        }

        if(CanSwitch==false){
            setTexture(mTextureHandle, cmdBuf);
            mDeviceFunctions->vkCmdBindVertexBuffers(cmdBuf, 0, 1, &(*it)->getVBuffer(), &vbOffset);
            setModelMatrix(mCamera.cMatrix() * (*it)->mMatrix);
            mDeviceFunctions->vkCmdDraw(cmdBuf, (*it)->mVertices.size(), 1, 0, 0);
        }
        else{
            setTexture(mTextureHandle, cmdBuf);
            mDeviceFunctions->vkCmdBindVertexBuffers(cmdBuf, 0, 1, &(*it)->getVBuffer(), &vbOffset);
            setModelMatrix(insideCamera.cMatrix() * (*it)->mMatrix);
            mDeviceFunctions->vkCmdDraw(cmdBuf, (*it)->mVertices.size(), 1, 0, 0);
        }
    }


    // Alternativt draw kall ved å traversere unordered map
    /*    for (auto it=mMap.begin(); it!=mMap.end(); it++)
    {
        // first er name, second er VisualObject*
        auto p = (*it).second;
        mDeviceFunctions->vkCmdBindVertexBuffers(cmdBuf, 0, 1, &p->mBuffer, &vbOffset);
        setModelMatrix(mCamera.cMatrix() * p->mMatrix);
        mDeviceFunctions->vkCmdDraw(cmdBuf, p->mVertices.size(), 1, 0, 0);
    }
*/
    mDeviceFunctions->vkCmdEndRenderPass(cmdBuf);
    // mObjects.at(0)->move(0.0f, 1.0f, 0.0f);
    //mObjects.at(2)->rotate(0.0f, 0.0f, 1.0f, 0.0f);
    // mObjects.at(2)->scale(0.5);
    // for (int i=0;i<10;i++){
    //     mObjects.at(2)->move(0.0f, 0.01f, 0.0f);
    //     break;
    // }




    //check collision with the door
    checkCollision(mObjects.at(1),mObjects.at(5));
    if (DoorIsOpen){
        //qDebug("You can enter");
        mObjects.at(5)->move(0.0f,-0.1f,0.0f); //door object
        // //check collision with the entrance, i use just first created object of wall, because middle point is calculated in relation to the whole house either way, so it doesnt matter which wall i'm using
        checkCollision(mObjects.at(1),mObjects.at(2));
        if (HouseEntered){
            CanSwitch=true;
        }
    }
    else{

    }


    //NPC patrol, next frame secures that new vertex positions are drawn in real time
    // 12 is NPC index, 1.0f patrol speed;
    Patrol(0.01f,mObjects.at(12), -2.0f, 2.0f);
    Patrol(0.02f,mObjects.at(13), -3.0f,1.0f);


    mWindow->frameReady();
    mWindow->requestUpdate(); // render continuously, throttled by the presentation rate
}

VkShaderModule Renderer::createShader(const QString &name)
{
    //This uses Qt's own file opening and resource system
    //We probably will replace it with pure C++ when expanding the program
    QFile file(name);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Failed to read shader %s", qPrintable(name));
        return VK_NULL_HANDLE;
    }
    QByteArray blob = file.readAll();
    file.close();

    VkShaderModuleCreateInfo shaderInfo{};
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo.codeSize = blob.size();
    shaderInfo.pCode = reinterpret_cast<const uint32_t *>(blob.constData());
    VkShaderModule shaderModule;
    VkResult err = mDeviceFunctions->vkCreateShaderModule(mWindow->device(), &shaderInfo, nullptr, &shaderModule);
    if (err != VK_SUCCESS) {
        qWarning("Failed to create shader module: %d", err);
        return VK_NULL_HANDLE;
    }

    return shaderModule;
}

void Renderer::setModelMatrix(QMatrix4x4 modelMatrix)
{

	mDeviceFunctions->vkCmdPushConstants(mWindow->currentCommandBuffer(), mPipelineLayout, 
        VK_SHADER_STAGE_VERTEX_BIT, 0, 16 * sizeof(float), modelMatrix.constData());
}

// Dag 240125
// This function contains some of the body of our former Renderer::initResources() function
// If we want to have more objects, we need to initialize buffers for each of them
// This version is not a version with encapsulation
// We use the VisualObject members mBuffer and mBufferMemory
void Renderer::createBuffer(VkDevice logicalDevice, const VkDeviceSize uniAlign,
                                VisualObject* visualObject, VkBufferUsageFlags usage)
{
    //Gets the size of the mesh - aligned to the uniform alignment
    VkDeviceSize vertexAllocSize = aligned(visualObject->getVertices().size() * sizeof(Vertex), uniAlign);

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; // Set the structure type
    bufferInfo.size = vertexAllocSize; //One vertex buffer (we don't use Uniform buffer in this example)
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; // Set the usage vertex buffer (not using Uniform buffer in this example)

    VkResult err = mDeviceFunctions->vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &visualObject->getVBuffer());
    if (err != VK_SUCCESS)
        qFatal("Failed to create buffer: %d", err);

    VkMemoryRequirements memReq;
    mDeviceFunctions->vkGetBufferMemoryRequirements(logicalDevice, visualObject->getVBuffer(), &memReq);

    VkMemoryAllocateInfo memAllocInfo{};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.pNext = nullptr;
    memAllocInfo.allocationSize = memReq.size;
    memAllocInfo.memoryTypeIndex = mWindow->hostVisibleMemoryIndex();

    err = mDeviceFunctions->vkAllocateMemory(logicalDevice, &memAllocInfo, nullptr, &visualObject->getVBufferMemory());
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate memory: %d", err);

    err = mDeviceFunctions->vkBindBufferMemory(logicalDevice, visualObject->getVBuffer(), visualObject->getVBufferMemory(), 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind buffer memory: %d", err);

    void* p{nullptr};
    err = mDeviceFunctions->vkMapMemory(logicalDevice, visualObject->getVBufferMemory(), 0, memReq.size, 0, reinterpret_cast<void **>(&p));
    if (err != VK_SUCCESS)
        qFatal("Failed to map memory: %d", err);

    memcpy(p, visualObject->getVertices().data(), visualObject->getVertices().size()*sizeof(Vertex));

    mDeviceFunctions->vkUnmapMemory(logicalDevice, visualObject->getVBufferMemory());
}

void Renderer::createVertexBuffer(const VkDeviceSize uniformAlignment, VisualObject *visualObject)
{
    //Get the size of the mesh and align it to the uniform alignment
    VkDeviceSize vertexAllocSize = aligned(visualObject->getVertices().size() * sizeof(Vertex), uniformAlignment);

    BufferHandle stagingHandle = createGeneralBuffer(vertexAllocSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, //Transfer source bit is for copying data to the GPU
                                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);    // Host visible memory (CPU) is slower to access than device local memory (GPU)

    void* data{ nullptr };
    mDeviceFunctions->vkMapMemory(mWindow->device(), stagingHandle.mBufferMemory, 0, vertexAllocSize, 0, &data);
    memcpy(data, visualObject->getVertices().data(), vertexAllocSize);
    mDeviceFunctions->vkUnmapMemory(mWindow->device(), stagingHandle.mBufferMemory);

    //This is for copying the data to the GPU
    BufferHandle gpuHandle = createGeneralBuffer(vertexAllocSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT); // Device local memory (GPU VRam) is faster to access than host visible memory (CPU RAM)

    //Set the buffer and buffer memory in the VisualObject for use in the draw call
    visualObject->setVBuffer(gpuHandle.mBuffer);
    visualObject->setVBufferMemory(gpuHandle.mBufferMemory);

    //Copy the data from the staging buffer to the GPU buffer
    VkCommandBuffer commandBuffer = BeginTransientCommandBuffer();
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = vertexAllocSize;
    mDeviceFunctions->vkCmdCopyBuffer(commandBuffer, stagingHandle.mBuffer, gpuHandle.mBuffer, 1, &copyRegion);
    EndTransientCommandBuffer(commandBuffer);

    //Free the staging buffer
    DestroyBuffer(stagingHandle);

}

void Renderer::createIndexBuffer(const VkDeviceSize uniformAlignment, VisualObject *visualObject)
{
    //Get the size of the mesh and align it to the uniform alignment
    VkDeviceSize indexAllocSize = aligned(visualObject->getIndices().size() * sizeof(uint32_t), uniformAlignment);

    //Create a staging buffer for the index data
    BufferHandle stagingHandle = createGeneralBuffer(indexAllocSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);    // Host visible memory (CPU) is slower to access than device local memory (GPU)

    void* data{ nullptr };
    mDeviceFunctions->vkMapMemory(mWindow->device(), stagingHandle.mBufferMemory, 0, indexAllocSize, 0, &data);
    memcpy(data, visualObject->getIndices().data(), indexAllocSize);
    mDeviceFunctions->vkUnmapMemory(mWindow->device(), stagingHandle.mBufferMemory);

    //This is for copying the data to the GPU
    BufferHandle gpuHandle = createGeneralBuffer(indexAllocSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT); // Device local memory (GPU VRam) is faster to access than host visible memory (CPU RAM)

    //Set the buffer and buffer memory in the VisualObject for use in the draw call
    visualObject->setIBuffer(gpuHandle.mBuffer);
    visualObject->setIBufferMemory(gpuHandle.mBufferMemory);

    //Copy the data from the staging buffer to the GPU buffer:
    VkCommandBuffer commandBuffer = BeginTransientCommandBuffer();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = indexAllocSize;
    mDeviceFunctions->vkCmdCopyBuffer(commandBuffer, stagingHandle.mBuffer, gpuHandle.mBuffer, 1, &copyRegion);
    EndTransientCommandBuffer(commandBuffer);

    //Free the staging buffer
    DestroyBuffer(stagingHandle);

}

BufferHandle Renderer::createGeneralBuffer(const VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{

    BufferHandle bufferHandle{};

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;    // set the structure type
    bufferInfo.size = size;                                     // size of the wanted buffer
    bufferInfo.usage = usage;                                   // buffer usage type
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult err = mDeviceFunctions->vkCreateBuffer(mWindow->device(), &bufferInfo, nullptr, &bufferHandle.mBuffer);
    if (err != VK_SUCCESS)
    {
        qFatal("Failed to create general buffer: %d", err);
    }

    VkMemoryRequirements memoryRequirements{};
    mDeviceFunctions->vkGetBufferMemoryRequirements(mWindow->device(), bufferHandle.mBuffer, &memoryRequirements);

    // Manually find a memory type
    uint32_t chosenMemoryType = findMemoryType(memoryRequirements.memoryTypeBits, properties);

    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = chosenMemoryType;      //Qt has a helper function for this mWindow->hostVisibleMemoryIndex();

    err = mDeviceFunctions->vkAllocateMemory(mWindow->device(), &memoryAllocateInfo, nullptr, &bufferHandle.mBufferMemory);
    if (err != VK_SUCCESS)
    {
        qFatal("Failed to allocate buffer memory: %d", err);
    }

    mDeviceFunctions->vkBindBufferMemory(mWindow->device(), bufferHandle.mBuffer, bufferHandle.mBufferMemory, 0);

    return bufferHandle;
}

void Renderer::DestroyBuffer(BufferHandle handle)
{
    mDeviceFunctions->vkDeviceWaitIdle(mWindow->device());
    mDeviceFunctions->vkDestroyBuffer(mWindow->device(), handle.mBuffer, nullptr);
    mDeviceFunctions->vkFreeMemory(mWindow->device(), handle.mBufferMemory, nullptr);

}

uint32_t Renderer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags requiredProperties)
{

    VkPhysicalDeviceMemoryProperties memoryProperties;

    // Get the QVulkanFunctions instance - pretty hidden in Qt
    QVulkanFunctions* vulkanFunctions = mWindow->vulkanInstance()->functions();
    vulkanFunctions->vkGetPhysicalDeviceMemoryProperties(mWindow->physicalDevice(), &memoryProperties);
    std::vector<VkMemoryType> memoryTypes; //getting the memory types
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        memoryTypes.push_back(memoryProperties.memoryTypes[i]);
    }

    //uint32_t chosenMemoryType{ 0 };
    for (uint32_t i = 0; i < memoryTypes.size(); i++)
    {
        bool isSuitable = (typeFilter & (1 << i));
        //CPU memory
        bool isHostVisible = (memoryTypes[i].propertyFlags & requiredProperties);
        if (isSuitable && isHostVisible)
        {
            return i;
        }
    }
    qFatal("Failed to find memory type! This will crash!");

    return 0;
}

VkCommandBuffer Renderer::BeginTransientCommandBuffer()
{
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = mWindow->graphicsCommandPool();
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    mDeviceFunctions->vkAllocateCommandBuffers(mWindow->device(), &allocateInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    mDeviceFunctions->vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;

}

void Renderer::EndTransientCommandBuffer(VkCommandBuffer commandBuffer)
{
    mDeviceFunctions->vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    //This is the way to submit a command buffer in Vulkan
    mDeviceFunctions->vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    //mDeviceFunctions->vkQueueWaitIdle(mGraphicsQueue);
    mDeviceFunctions->vkFreeCommandBuffers(mWindow->device(), mWindow->graphicsCommandPool(), 1, &commandBuffer);
}

void Renderer::createUniformBuffer()
{
    VkDeviceSize bufferSize = 64 + 64;      // two 4x4 matrices

    mUniformBuffer = createGeneralBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    //Map the buffer memory
    VkResult err = mDeviceFunctions->vkMapMemory(mWindow->device(), mUniformBuffer.mBufferMemory, 0, bufferSize, 0, &mUniformBufferLocation);
    if (err != VK_SUCCESS)
        qFatal("Failed to map memory: %d", err);
}

void Renderer::createDescriptorSetLayouts()
{
    VkDescriptorSetLayoutBinding uniformLayoutBinding{};
    uniformLayoutBinding.binding = 0;
    uniformLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformLayoutBinding.descriptorCount = 1;
    uniformLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;   //We are using the uniform buffer in the vertex shader

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uniformLayoutBinding;

    VkResult err = mDeviceFunctions->vkCreateDescriptorSetLayout(mWindow->device(), &layoutInfo, nullptr, &mDescriptorSetLayout);
    if (err != VK_SUCCESS)
        qFatal("Failed to create DescriptorSetLayout: %d", err);

    //Textures
    VkDescriptorSetLayoutBinding textureLayoutBinding{};
    textureLayoutBinding.binding = 0;
    textureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textureLayoutBinding.descriptorCount = 1;
    textureLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;   //We are using the uniform buffer in the vertex shader

    VkDescriptorSetLayoutCreateInfo textureLayoutInfo{};
    textureLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    textureLayoutInfo.bindingCount = 1;
    textureLayoutInfo.pBindings = &textureLayoutBinding;

    err = mDeviceFunctions->vkCreateDescriptorSetLayout(mWindow->device(), &textureLayoutInfo, nullptr, &mTextureDescriptorSetLayout);
    if (err != VK_SUCCESS)
        qFatal("Failed to create TextureDescriptorSetLayout: %d", err);
}

void Renderer::createDescriptorSet()
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = mDescriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &mDescriptorSetLayout;

    VkResult err = mDeviceFunctions->vkAllocateDescriptorSets(mWindow->device(), &allocInfo, &mDescriptorSet);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate descriptor set: %d", err);

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = mUniformBuffer.mBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = 64 + 64;      // two 4x4 matrices

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = mDescriptorSet;        //[0];
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    mDeviceFunctions->vkUpdateDescriptorSets(mWindow->device(), 1, &descriptorWrite, 0, nullptr);
}

void Renderer::createDescriptorPools()
{
    VkDescriptorPoolSize uniformPoolSize{};
    uniformPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;  //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
    uniformPoolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo uniformPoolInfo{};
    uniformPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    uniformPoolInfo.maxSets = 1;
    uniformPoolInfo.poolSizeCount = 1;
    uniformPoolInfo.pPoolSizes = &uniformPoolSize;

    VkResult err = mDeviceFunctions->vkCreateDescriptorPool(mWindow->device(), &uniformPoolInfo, nullptr, &mDescriptorPool);
    if (err != VK_SUCCESS)
        qFatal("Failed to create descriptor pool: %d", err);


    //For Textures
    VkDescriptorPoolSize texturePoolSize{};
    texturePoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    texturePoolSize.descriptorCount = 1024;      // can ask the GPU - properties.limits.maxSamplerAllocationCount;

    VkDescriptorPoolCreateInfo texturePoolInfo{};
    texturePoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    texturePoolInfo.maxSets = 1024;             // can ask the GPU - properties.limits.maxDescriptorSetSamplers;
    texturePoolInfo.poolSizeCount = 1;
    texturePoolInfo.pPoolSizes = &texturePoolSize;
    texturePoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    err = mDeviceFunctions->vkCreateDescriptorPool(mWindow->device(), &texturePoolInfo, nullptr, &mTextureDescriptorPool);
    if (err != VK_SUCCESS)
        qFatal("Failed to create descriptor pool: %d", err);
}

void Renderer::setTexture(TextureHandle &textureHandle, VkCommandBuffer commandBuffer)
{
    mDeviceFunctions->vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
    mPipelineLayout, 1, 1, &textureHandle.mTextureDescriptorSet, 0, nullptr);
}

void Renderer::setViewProjectionMatrix()
{
    memcpy(mUniformBufferLocation, mCamera.viewMatrix().constData(), 64);
    QMatrix4x4 temp = mCamera.projectionMatrix();
    temp = temp * mWindow->clipCorrectionMatrix();  //Correcting for Vulkans -Y
    //Adding 64 bytes to the uniform buffer location to get to the projection matrix position
    memcpy(static_cast<char*>(mUniformBufferLocation) + 64, temp.constData(), 64);
}

void Renderer::createTextureSampler()
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_NEAREST;                   // Magnification filter - try VK_FILTER_LINEAR
    samplerInfo.minFilter = VK_FILTER_NEAREST;                   // Minification filter - try VK_FILTER_LINEAR
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;  // Address mode for U coordinates
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;  // Address mode for V coordinates
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;  // Address mode for W coordinates
    samplerInfo.anisotropyEnable = VK_FALSE;                     // Enable anisotropy
    samplerInfo.maxAnisotropy = 1.0;                             // Anisotropy level
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK; // Border color
    samplerInfo.unnormalizedCoordinates = VK_FALSE;             // Normalized coordinates
    samplerInfo.compareEnable = VK_FALSE;                       // Compare enable
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;               // Compare operation
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;     // Mipmap mode
    samplerInfo.mipLodBias = 0.0f;                              // Mipmap level of detail bias
    samplerInfo.minLod = 0.0f;                                  // Minimum level of detail
    samplerInfo.maxLod = 0.0f;                                  // Maximum level of detail

    VkResult err = mDeviceFunctions->vkCreateSampler(mWindow->device(), &samplerInfo, nullptr, &mTextureSampler);
    if (err != VK_SUCCESS){
        qFatal("Failed to create texture sampler: %d", err);
    }
}

TextureHandle Renderer::createTexture(const char *filename)
{

    int texWidth, texHeight, texChannels;
    VkDeviceSize bufferSize{};
    VkFormat format{ VK_FORMAT_R8G8B8A8_SRGB }; //could be VK_FORMAT_R8G8B8_SRGB
    BufferHandle stagingBuffer{};
    stbi_uc* pixelData{ nullptr };

    //Open the file and read the data into the imageFileData vector
    ifstream file(filename, ios::binary);

    //if the file is not open, we create a default texture
    if (!file.is_open())
    {   //TEXTURE
        // Texture* texture = new Texture();   //new Texture(filename);
        // bufferSize = texture->textureSize();
        // texChannels = texture->bytesPrPixel();
        // texWidth = texture->width();
        // texHeight = texture->height();


        //HEIGHT MAP
        // texture 2x2 pixels, 4 bytes per pixel
        pixelData = new stbi_uc[16]{};  //stbi_uc == unsigned char

        //Set some colors - alpha to 255
        pixelData[0] = 255;
        pixelData[3] = 255; //alpha
        pixelData[5] = 255;
        pixelData[7] = 255; //alpha
        pixelData[10] = 255;
        pixelData[11] = 255; //alpha
        pixelData[12] = 255;
        pixelData[13] = 255;
        pixelData[15] = 255; //alpha

        bufferSize = 16;  // 2 * 2 * 4 bytes
        texChannels = 4;
        texWidth = 2;
        texHeight = 2;
        stagingBuffer = createGeneralBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);


        void* data{};
        mDeviceFunctions->vkMapMemory(mWindow->device(), stagingBuffer.mBufferMemory, 0, bufferSize, 0, &data);

        //TEXTURE
        //if the file is open, we read the data into the imageFileData vector
        //memcpy(data, texture->getPixels(), bufferSize);

        //HEIGHT MAP
        //if the file is open, we read the data into the imageFileData vector using stb_image
        memcpy(data, pixelData, bufferSize);
    }


    else
    {
        const std::uint32_t size = std::filesystem::file_size(filename);
        std::vector<std::uint8_t> imageFileData(size);
        file.read(reinterpret_cast<char*>(imageFileData.data()), size);

        //Use the stb_image library to load the image
        pixelData = stbi_load_from_memory(imageFileData.data(), size, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        bufferSize = texChannels * texWidth * texHeight;
        stagingBuffer = createGeneralBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void* data{};
        mDeviceFunctions->vkMapMemory(mWindow->device(), stagingBuffer.mBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, pixelData, bufferSize);
    }

    /***************** HEIGHT MAP ***************/
    //Test to look at the pixel data values in the image
    //Jumping through the pixel data by 1200 bytes at a time to get a sample of the data
    //We see that in a grey scale image, the R, G, B values are the same! The A value is 255

    unsigned char temp{};
    for (int i = 0; i < texWidth * texHeight; i += 1200)
    {
        temp = pixelData[i];
        qDebug() << "Pixel " << i << "r " << temp;
        temp = pixelData[i + 1];
        qDebug() << "Pixel " << i << "g " << temp;
        temp = pixelData[i + 2];
        qDebug() << "Pixel " << i << "b " << temp;
        temp = pixelData[i + 3];
        qDebug() << "Pixel " << i << "a " << temp;
    }
    // /////////////////////////////////////////////////////////////////////////////////////////////////////////

    mDeviceFunctions->vkUnmapMemory(mWindow->device(), stagingBuffer.mBufferMemory);

    TextureHandle textureHandle = createImage(texWidth, texHeight, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, format);

    transitionImageLayout(textureHandle.mImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(stagingBuffer.mBuffer, textureHandle.mImage, texWidth, texHeight);
    transitionImageLayout(textureHandle.mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    textureHandle.mImageView = createImageView(textureHandle.mImage, format);

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = mTextureDescriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &mTextureDescriptorSetLayout;

    VkResult err = mDeviceFunctions->vkAllocateDescriptorSets(mWindow->device(), &descriptorSetAllocateInfo, &textureHandle.mTextureDescriptorSet);
    if (err != VK_SUCCESS) {
        std::exit(EXIT_FAILURE);
    }

    VkDescriptorImageInfo descriptorImageInfo{};
    descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    descriptorImageInfo.imageView = textureHandle.mImageView;
    descriptorImageInfo.sampler = mTextureSampler;

    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = textureHandle.mTextureDescriptorSet;
    writeDescriptorSet.dstBinding = 0;
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.pImageInfo = &descriptorImageInfo;

    mDeviceFunctions->vkUpdateDescriptorSets(mWindow->device(), 1, &writeDescriptorSet, 0, nullptr);

    DestroyBuffer(stagingBuffer);

    stbi_image_free(pixelData);

    return textureHandle;
}

TextureHandle Renderer::createImage(int width, int height, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkFormat format)
{
    TextureHandle textureHandle{};

    VkImageCreateInfo textureInfo{};
    textureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;    // set the structure type
    textureInfo.usage = usage;                                   // buffer usage type
    textureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    textureInfo.imageType = VK_IMAGE_TYPE_2D;
    textureInfo.extent.width = width;
    textureInfo.extent.height = height;
    textureInfo.extent.depth = 1;
    textureInfo.mipLevels = 1;
    textureInfo.arrayLayers = 1;
    textureInfo.format = format;
    textureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    textureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //textureInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    textureInfo.samples = VK_SAMPLE_COUNT_1_BIT;        // No multisampling
    //textureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    textureInfo.flags = 0;

    VkResult err = mDeviceFunctions->vkCreateImage(mWindow->device(), &textureInfo, nullptr, &textureHandle.mImage);
    if (err != VK_SUCCESS)
    {
        qFatal("Failed to create image buffer: %d", err);
    }

    VkMemoryRequirements memoryRequirements{};
    mDeviceFunctions->vkGetImageMemoryRequirements(mWindow->device(), textureHandle.mImage, &memoryRequirements);

    // Manually find a memory type
    uint32_t chosenMemoryType = findMemoryType(memoryRequirements.memoryTypeBits, properties);

    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = chosenMemoryType;      //Qt has a helper function for this mWindow->hostVisibleMemoryIndex();

    err = mDeviceFunctions->vkAllocateMemory(mWindow->device(), &memoryAllocateInfo, nullptr, &textureHandle.mTextureMemory);
    if (err != VK_SUCCESS)
    {
        qFatal("Failed to allocate image memory: %d", err);
    }

    mDeviceFunctions->vkBindImageMemory(mWindow->device(), textureHandle.mImage, textureHandle.mTextureMemory, 0);

    return textureHandle;
}

void Renderer::transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = BeginTransientCommandBuffer();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage{};
    VkPipelineStageFlags destinationStage{};

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }

    mDeviceFunctions->vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    EndTransientCommandBuffer(commandBuffer);
}

void Renderer::copyBufferToImage(VkBuffer buffer, VkImage image, int width, int height)
{
    VkCommandBuffer commandBuffer = BeginTransientCommandBuffer();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height), 1 };

    mDeviceFunctions->vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    EndTransientCommandBuffer(commandBuffer);
}

VkImageView Renderer::createImageView(VkImage image, VkFormat format)
{
    VkImageViewCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.image = image;
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.format = format;
    info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;

    VkImageView view;
    VkResult err = mDeviceFunctions->vkCreateImageView(mWindow->device(), &info, nullptr, &view);
    if (err != VK_SUCCESS) {
        std::exit(EXIT_FAILURE);
    }

    return view;
}

void Renderer::destroyTexture(TextureHandle &textureHandle)
{
    mDeviceFunctions->vkDeviceWaitIdle(mWindow->device());
    mDeviceFunctions->vkFreeDescriptorSets(mWindow->device(), mTextureDescriptorPool, 1, &textureHandle.mTextureDescriptorSet);
    mDeviceFunctions->vkDestroyImageView(mWindow->device(), textureHandle.mImageView, nullptr);
    mDeviceFunctions->vkDestroyImage(mWindow->device(), textureHandle.mImage, nullptr);
    mDeviceFunctions->vkFreeMemory(mWindow->device(), textureHandle.mTextureMemory, nullptr);
}

void Renderer::getVulkanHWInfo()
{
    qDebug("\n ***************************** Vulkan Hardware Info ******************************************* \n");
    QVulkanInstance *inst = mWindow->vulkanInstance();
    mDeviceFunctions = inst->deviceFunctions(mWindow->device());

    QString info;
    info += QString::asprintf("Number of physical devices: %d\n", int(mWindow->availablePhysicalDevices().count()));

    QVulkanFunctions *f = inst->functions();
    VkPhysicalDeviceProperties props;
    f->vkGetPhysicalDeviceProperties(mWindow->physicalDevice(), &props);
    info += QString::asprintf("Active physical device name: '%s' version %d.%d.%d\nAPI version %d.%d.%d\n",
                              props.deviceName,
                              VK_VERSION_MAJOR(props.driverVersion), VK_VERSION_MINOR(props.driverVersion),
                              VK_VERSION_PATCH(props.driverVersion),
                              VK_VERSION_MAJOR(props.apiVersion), VK_VERSION_MINOR(props.apiVersion),
                              VK_VERSION_PATCH(props.apiVersion));

    info += QStringLiteral("Supported instance layers:\n");
    for (const QVulkanLayer &layer : inst->supportedLayers())
        info += QString::asprintf("    %s v%u\n", layer.name.constData(), layer.version);
    info += QStringLiteral("Enabled instance layers:\n");
    for (const QByteArray &layer : inst->layers())
        info += QString::asprintf("    %s\n", layer.constData());

    info += QStringLiteral("Supported instance extensions:\n");
    for (const QVulkanExtension &ext : inst->supportedExtensions())
        info += QString::asprintf("    %s v%u\n", ext.name.constData(), ext.version);
    info += QStringLiteral("Enabled instance extensions:\n");
    for (const QByteArray &ext : inst->extensions())
        info += QString::asprintf("    %s\n", ext.constData());

    info += QString::asprintf("Color format: %u\nDepth-stencil format: %u\n",
                              mWindow->colorFormat(), mWindow->depthStencilFormat());

    info += QStringLiteral("Supported sample counts:");
    const QList<int> sampleCounts = mWindow->supportedSampleCounts();
    for (int count : sampleCounts)
        info += QLatin1Char(' ') + QString::number(count);
    info += QLatin1Char('\n');

    qDebug(info.toUtf8().constData());
    qDebug("\n ***************************** Vulkan Hardware Info finished ******************************************* \n");
}

void Renderer::releaseSwapChainResources()
{
    qDebug("\n ***************************** releaseSwapChainResources ******************************************* \n");
}

void Renderer::releaseResources()
{
    qDebug("\n ***************************** releaseResources ******************************************* \n");

    VkDevice dev = mWindow->device();

    if (mPipeline) {
        mDeviceFunctions->vkDestroyPipeline(dev, mPipeline, nullptr);
        mPipeline = VK_NULL_HANDLE;
    }

    if (mPipelineLayout) {
        mDeviceFunctions->vkDestroyPipelineLayout(dev, mPipelineLayout, nullptr);
        mPipelineLayout = VK_NULL_HANDLE;
    }

    if (mPipelineCache) {
        mDeviceFunctions->vkDestroyPipelineCache(dev, mPipelineCache, nullptr);
        mPipelineCache = VK_NULL_HANDLE;
    }

    if (mDescriptorSetLayout) {
        mDeviceFunctions->vkDestroyDescriptorSetLayout(dev, mDescriptorSetLayout, nullptr);
        mDescriptorSetLayout = VK_NULL_HANDLE;
    }

    if (mDescriptorPool) {
        mDeviceFunctions->vkDestroyDescriptorPool(dev, mDescriptorPool, nullptr);
        mDescriptorPool = VK_NULL_HANDLE;
    }

    if (mVisualObject.mBuffer) {
        mDeviceFunctions->vkDestroyBuffer(dev, mVisualObject.mBuffer, nullptr);
        mVisualObject.mBuffer = VK_NULL_HANDLE;
    }

    if (mVisualObject.mBufferMemory) {
        mDeviceFunctions->vkFreeMemory(dev, mVisualObject.mBufferMemory, nullptr);
        mVisualObject.mBufferMemory = VK_NULL_HANDLE;
    }
    // Samme for alle objekter i container
    for (auto it=mObjects.begin(); it!=mObjects.end(); it++) {
        if ((*it)->mBuffer) {
            mDeviceFunctions->vkDestroyBuffer(dev, (*it)->mBuffer, nullptr);
            (*it)->mBuffer = VK_NULL_HANDLE;
        }
    }
    for (auto it=mObjects.begin(); it!=mObjects.end(); it++) {
        if ((*it)->mBufferMemory) {
            mDeviceFunctions->vkFreeMemory(dev, (*it)->mBufferMemory, nullptr);
            (*it)->mBuffer = VK_NULL_HANDLE;
        }

    }

    for (auto it=mPickups.begin(); it!=mPickups.end(); it++) {
        if ((*it)->mBuffer) {
            mDeviceFunctions->vkDestroyBuffer(dev, (*it)->mBuffer, nullptr);
            (*it)->mBuffer = VK_NULL_HANDLE;
        }
    }
    for (auto it=mPickups.begin(); it!=mPickups.end(); it++) {
        if ((*it)->mBufferMemory) {
            mDeviceFunctions->vkFreeMemory(dev, (*it)->mBufferMemory, nullptr);
            (*it)->mBuffer = VK_NULL_HANDLE;
        }

    }
}

VisualObject* Renderer::checkCollision(VisualObject* v1, VisualObject* v2){
    Vertex distance;
    distance.x=v1->getMiddlePoints(0).x- v2->getMiddlePoints(0).x;
    distance.y=v1->getMiddlePoints(0).y- v2->getMiddlePoints(0).y;
    distance.z=v1->getMiddlePoints(0).z- v2->getMiddlePoints(0).z;
    distance.r=0.0f;
    distance.g=0.0f;
    distance.g=0.0f;
    distance.u=0.0f;
    distance.v=0.0f;
    float distance_length=qSqrt(qPow(distance.x,2) +qPow(distance.y,2) + qPow(distance.z,2));
    float radius1 =v1->radius;
    float radius2 =v2->radius;
    if (distance_length<=(radius1+radius2)){
        //qDebug("They collide");
        IsColliding=true;
        if (v2->CollisionType==1){
            DoorIsOpen=true;
        }

        else if (DoorIsOpen && v2->CollisionType==2){
            HouseEntered=true;
        }
        else{

        }
    }
    else{
        IsColliding=false;
    }
    //qDebug()<<distance_length;

    return v2;
}



void Renderer::Patrol(float speed, VisualObject* ptr, float min, float max)
{
    float right_min=min;
    float right_max=max;

    if (ptr->MovingRight){
        for(auto it=ptr->mVertices.begin(); it!=ptr->mVertices.end();it++){
            (*it).x+=speed;
        }
        ptr->updateMiddlePoints(0,speed,0.0f,0.0f);
        ptr->mMatrix.translate(speed,0.0f,0.0f);
        if (ptr->mVertices.at(0).x >=right_max){
        ptr->MovingRight=false;
        }
    }
    else{
        for(auto it=ptr->mVertices.begin(); it!=ptr->mVertices.end();it++){
            (*it).x-=speed;    
        }
        ptr->updateMiddlePoints(0,-speed,0.0f,0.0f);
        ptr->mMatrix.translate(-speed,0.0f,0.0f);
        if (ptr->mVertices.at(0).x<=right_min){
        ptr->MovingRight=true;
        }
    }

    VisualObject* temp=checkCollision(mObjects.at(1),ptr);
    if (IsColliding && temp==ptr){
        qDebug("You lost");
        CanMove=false;
    }

};



