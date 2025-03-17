#ifndef UTILITIES_H
#define UTILITIES_H

#include <QVulkanFunctions>

//Utility function for alignment:
static inline VkDeviceSize aligned(VkDeviceSize v, VkDeviceSize byteAlign)
{
    return (v + byteAlign - 1) & ~(byteAlign - 1);
}

//Utility struct for handling buffers
struct BufferHandle
{
    VkDeviceMemory mBufferMemory{ VK_NULL_HANDLE };
    VkBuffer mBuffer{ VK_NULL_HANDLE };
};

#endif // UTILITIES_H
