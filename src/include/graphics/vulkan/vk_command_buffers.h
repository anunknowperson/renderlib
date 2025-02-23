#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "vk_types.h"
#include "vk_initializers.h"

class CommandBuffers {
public:
    CommandBuffers(VkDevice device, uint32_t queueFamilyIndex);
    ~CommandBuffers();

    void initCommandBuffers(size_t frameCount);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue queue);
    VkCommandBuffer getCommandBuffer(size_t frameIndex) const;
    void cleanup();

private:
    VkDevice _device;
    uint32_t _queueFamilyIndex;
    VkCommandPool _commandPool;
    std::vector<VkCommandBuffer> _commandBuffers;
}
