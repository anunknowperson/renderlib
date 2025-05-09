#pragma once

#include "vk_types.h"
#include "vk_pipelines.h"
#include <glm/glm.hpp>

struct DrawContext;
class VulkanEngine;

class VulkanRenderer {
public:
    VulkanRenderer() = default;
    
    void init(VulkanEngine* engine);

    // Main drawing method that coordinates the entire frame
    void draw_frame(VkCommandBuffer cmd, VkImageView targetImageView, uint32_t swapchainImageIndex);
    
    // Individual drawing methods
    void draw_background(VkCommandBuffer cmd) const;
    void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView) const;
    void draw_geometry(VkCommandBuffer cmd);

private:
    VulkanEngine* _engine = nullptr;
};