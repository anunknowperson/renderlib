#pragma once


#include <vulkan/vulkan.h>
#include <vector>
#include <functional>

class VulkanEngine;

class CommandBuffers {
public:

    void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function,
                          VulkanEngine* vk_engine) const;

    void init_commands(VulkanEngine* vk_engine);

private:

};