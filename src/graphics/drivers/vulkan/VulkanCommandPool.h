#pragma once

#include <memory>
#include <vector>

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"

class VulkanRender;

class VulkanCommandPool {
public:
    explicit VulkanCommandPool(VulkanRender* p_render);
    ~VulkanCommandPool();

    VkCommandPool commandPool;

    std::vector<VkCommandBuffer> commandBuffers;

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);


private:

    VulkanRender* render;

};