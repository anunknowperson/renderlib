#pragma once

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include <memory>

#include <vector>


#include "internal/vulkan_instance.h"
#include "internal/vulkan_physical_device.h"
#include "internal/vulkan_surface.h"

class VulkanRender;

class VulkanCommandPool {
private:

    VulkanRender* render;
public:

    VkCommandPool commandPool;

    std::vector<VkCommandBuffer> commandBuffers;

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    VulkanCommandPool(VulkanRender* p_render);
    ~VulkanCommandPool();
};