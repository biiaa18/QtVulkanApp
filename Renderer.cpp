 #include "Renderer.h"
#include <QVulkanFunctions>
#include <QFile>
#include <QtMath>
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


    const int concurrentFrameCount = mWindow->concurrentFrameCount(); // 2 on Oles Machine
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

    /********************************* Vertex layout: *********************************/
    VkVertexInputBindingDescription vertexBindingDesc ={};
    vertexBindingDesc.binding = 0;
    vertexBindingDesc.stride = sizeof(Vertex);
    vertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    /********************************* Shader bindings: *********************************/
    //Descritpion of the attributes used for vertices in the shader
    VkVertexInputAttributeDescription vertexAttrDesc[2];    //Updated to a more common way to write it
    vertexAttrDesc[0].location = 0;
    vertexAttrDesc[0].binding = 0;
    vertexAttrDesc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexAttrDesc[0].offset = 0;

    vertexAttrDesc[1].location = 1;
    vertexAttrDesc[1].binding = 0;
    vertexAttrDesc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexAttrDesc[1].offset = 3 * sizeof(float);           // could use offsetof(Vertex, r); from <cstddef>

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = nullptr;
    vertexInputInfo.flags = 0;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDesc;
    vertexInputInfo.vertexAttributeDescriptionCount = 2; // position and color
    vertexInputInfo.pVertexAttributeDescriptions = vertexAttrDesc;

    // Pipeline cache - supposed to increase performance
    VkPipelineCacheCreateInfo pipelineCacheInfo;
    memset(&pipelineCacheInfo, 0, sizeof(pipelineCacheInfo));
    pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    VkResult err = mDeviceFunctions->vkCreatePipelineCache(logicalDevice, &pipelineCacheInfo, nullptr, &mPipelineCache);
    if (err != VK_SUCCESS)
        qFatal("Failed to create pipeline cache: %d", err);

    // Pipeline layout
    // OEF: PushConstants update - set up the push constant info
    VkPushConstantRange pushConstantRange{
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        16 * sizeof(float) // 16 floats for the model matrix
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo;
    memset(&pipelineLayoutInfo, 0, sizeof(pipelineLayoutInfo));
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 1;  // OEF: PushConstants update
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // OEF: PushConstants update
    err = mDeviceFunctions->vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout);
    if (err != VK_SUCCESS)
        qFatal("Failed to create pipeline layout: %d", err);

    /********************************* Create shaders *********************************/
    //Creates our actuall shader modules
    VkShaderModule vertShaderModule = createShader(QStringLiteral(":/color_vert.spv"));
    VkShaderModule fragShaderModule = createShader(QStringLiteral(":/color_frag.spv"));

    // Graphics pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo;
    memset(&pipelineInfo, 0, sizeof(pipelineInfo));
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    VkPipelineShaderStageCreateInfo shaderStages[2] = {
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,   //sType  (structure type)
            nullptr,
            0,
            VK_SHADER_STAGE_VERTEX_BIT, //stage
            vertShaderModule, //module
            "main",   //pName
            nullptr
        },
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            nullptr,
            0,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            fragShaderModule,
            "main",
            nullptr
        }
    };

    pipelineInfo.stageCount = 2; //vertex and fragment shader
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;

    // **** Input Assembly **** - describes how primitives are assembled in the Graphics pipeline
    VkPipelineInputAssemblyStateCreateInfo ia;  //input assembly
    memset(&ia, 0, sizeof(ia));
    ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    // Dag 220125
    ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineInfo.pInputAssemblyState = &ia;

    // The viewport and scissor will be set dynamically via vkCmdSetViewport/Scissor.
    // This way the pipeline does not need to be touched when resizing the window.
    VkPipelineViewportStateCreateInfo vp;
    memset(&vp, 0, sizeof(vp));
    vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp.viewportCount = 1;
    vp.scissorCount = 1;
    pipelineInfo.pViewportState = &vp;

    // **** Rasterizer **** - takes the geometry and turns it into fragments
    VkPipelineRasterizationStateCreateInfo rs;  //rasterization
    memset(&rs, 0, sizeof(rs));
    rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.polygonMode = VK_POLYGON_MODE_FILL;//VK_POLYGON_MODE_LINE;
    rs.cullMode = VK_CULL_MODE_NONE; // we want the back face as well
    rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rs.lineWidth = 1.0f;
    pipelineInfo.pRasterizationState = &rs;

    VkPipelineMultisampleStateCreateInfo ms;
    memset(&ms, 0, sizeof(ms));
    ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    // Enable multisampling.
    ms.rasterizationSamples = mWindow->sampleCountFlagBits();
    pipelineInfo.pMultisampleState = &ms;

    // **** Dynamic State **** - dynamic states can be changed without recreating the pipeline

    VkPipelineDepthStencilStateCreateInfo ds;
    memset(&ds, 0, sizeof(ds));
    ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ds.depthTestEnable = VK_TRUE;
    ds.depthWriteEnable = VK_TRUE;
    ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    pipelineInfo.pDepthStencilState = &ds;

    VkPipelineColorBlendStateCreateInfo cb;
    memset(&cb, 0, sizeof(cb));
    cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    // no blend, write out all of rgba
    VkPipelineColorBlendAttachmentState att;
    memset(&att, 0, sizeof(att));
    att.colorWriteMask = 0xF;
    cb.attachmentCount = 1;
    cb.pAttachments = &att;
    pipelineInfo.pColorBlendState = &cb;

    VkDynamicState dynEnable[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dyn;
    memset(&dyn, 0, sizeof(dyn));
    dyn.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dyn.dynamicStateCount = sizeof(dynEnable) / sizeof(VkDynamicState);
    dyn.pDynamicStates = dynEnable;
    pipelineInfo.pDynamicState = &dyn;

    pipelineInfo.layout = mPipelineLayout;
    pipelineInfo.renderPass = mWindow->defaultRenderPass();

    err = mDeviceFunctions->vkCreateGraphicsPipelines(logicalDevice, mPipelineCache, 1, &pipelineInfo, nullptr, &mPipeline);
    if (err != VK_SUCCESS)
        qFatal("Failed to create graphics pipeline: %d", err);

    //Making a pipeline2 for drawing lines
    mPipeline2 = mPipeline;                                    //reusing most of the settings from the first pipeline
    ia.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;   // or VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    rs.polygonMode = VK_POLYGON_MODE_LINE;           // VK_POLYGON_MODE_LINE will make a wireframe; VK_POLYGON_MODE_FILL
    pipelineInfo.pInputAssemblyState = &ia;
    err = mDeviceFunctions->vkCreateGraphicsPipelines(logicalDevice, mPipelineCache, 1, &pipelineInfo, nullptr, &mPipeline);
    if (err != VK_SUCCESS)
        qFatal("Failed to create graphics pipeline: %d", err);

    if (vertShaderModule)
        mDeviceFunctions->vkDestroyShaderModule(logicalDevice, vertShaderModule, nullptr);
    if (fragShaderModule)
        mDeviceFunctions->vkDestroyShaderModule(logicalDevice, fragShaderModule, nullptr);

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

    const QSize sz = mWindow->swapChainImageSize();
    //qDebug() << "startNextFrame()";
    //Backtgound color of the render window - dark grey
    VkClearColorValue clearColor = {{ 0.3, 0.3, 0.3, 1 }};

    VkClearDepthStencilValue clearDS = { 1, 0 };
    VkClearValue clearValues[3];
    memset(clearValues, 0, sizeof(clearValues));
    clearValues[0].color = clearValues[2].color = clearColor;
    clearValues[1].depthStencil = clearDS;

    VkRenderPassBeginInfo rpBeginInfo;
    memset(&rpBeginInfo, 0, sizeof(rpBeginInfo));
    rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderPass = mWindow->defaultRenderPass();
    rpBeginInfo.framebuffer = mWindow->currentFramebuffer();
    rpBeginInfo.renderArea.extent.width = sz.width();
    rpBeginInfo.renderArea.extent.height = sz.height();
    rpBeginInfo.clearValueCount = mWindow->sampleCountFlagBits() > VK_SAMPLE_COUNT_1_BIT ? 3 : 2;
    rpBeginInfo.pClearValues = clearValues;
    mDeviceFunctions->vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    mDeviceFunctions->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);

    VkDeviceSize vbOffset = 0;

    VkViewport viewport{};
    viewport.x = viewport.y = 0;
    viewport.width = sz.width();
    viewport.height = sz.height();
    viewport.minDepth = 0;
    viewport.maxDepth = 1;
    mDeviceFunctions->vkCmdSetViewport(cmdBuf, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = viewport.width;
    scissor.extent.height = viewport.height;
    mDeviceFunctions->vkCmdSetScissor(cmdBuf, 0, 1, &scissor);

    /********************************* Our draw call!: *********************************/

    for (auto it=mObjects.begin(); it!=mObjects.end(); it++)
    {
        if ((*it)->drawType==0){
            //pipeline 2 for triangle list
            mDeviceFunctions->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline2);

        }
        else{
            //pipeline1 for line list
            mDeviceFunctions->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
        }
        // if camera can switch, we switch to insideCamera from mMatrix
        if(CanSwitch==false){
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
            mDeviceFunctions->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline2);

        }

        if(CanSwitch==false){
            mDeviceFunctions->vkCmdBindVertexBuffers(cmdBuf, 0, 1, &(*it)->getVBuffer(), &vbOffset);
            setModelMatrix(mCamera.cMatrix() * (*it)->mMatrix);
            mDeviceFunctions->vkCmdDraw(cmdBuf, (*it)->mVertices.size(), 1, 0, 0);
        }
        else{
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

    VkShaderModuleCreateInfo shaderInfo;
    memset(&shaderInfo, 0, sizeof(shaderInfo));
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
    VkBufferCreateInfo bufferInfo{};
    memset(&bufferInfo, 0, sizeof(bufferInfo)); //Clear out the memory
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; // Set the structure type

    VkDeviceSize vertexAllocSize = aligned(visualObject->getVertices().size()*sizeof(Vertex), uniAlign);
    bufferInfo.size = vertexAllocSize; //One vertex buffer (we don't use Uniform buffer in this example)
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; // Set the usage vertex buffer (not using Uniform buffer in this example)

    VkResult err = mDeviceFunctions->vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &visualObject->mBuffer);
    if (err != VK_SUCCESS)
        qFatal("Failed to create buffer: %d", err);

    VkMemoryRequirements memReq;
    mDeviceFunctions->vkGetBufferMemoryRequirements(logicalDevice, visualObject->mBuffer, &memReq);

    VkMemoryAllocateInfo memAllocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        memReq.size,
        mWindow->hostVisibleMemoryIndex()
    };

    err = mDeviceFunctions->vkAllocateMemory(logicalDevice, &memAllocInfo, nullptr, &visualObject->mBufferMemory);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate memory: %d", err);

    err = mDeviceFunctions->vkBindBufferMemory(logicalDevice, visualObject->mBuffer, visualObject->mBufferMemory, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind buffer memory: %d", err);

    quint8* p{nullptr};
    err = mDeviceFunctions->vkMapMemory(logicalDevice, visualObject->mBufferMemory, 0, memReq.size, 0, reinterpret_cast<void **>(&p));
    if (err != VK_SUCCESS)
        qFatal("Failed to map memory: %d", err);

    memcpy(p, visualObject->getVertices().data(), visualObject->getVertices().size()*sizeof(Vertex));

    mDeviceFunctions->vkUnmapMemory(logicalDevice, visualObject->mBufferMemory);
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

    VkMemoryRequirements memoryRequirements;
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



