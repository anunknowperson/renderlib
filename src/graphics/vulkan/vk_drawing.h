#pragma once

#include "vk_types.h"
#include "pipelines.h"
#include <memory>

class VulkanEngine;
struct DrawContext;

class Drawing {
public:
    Drawing(VulkanEngine* engine);
    ~Drawing() = default;

    // Main draw function
    void draw();

private:
    // Drawing components
    void drawBackground(VkCommandBuffer cmd) const;
    void drawImGui(VkCommandBuffer cmd, VkImageView targetImageView) const;
    void drawGeometry(VkCommandBuffer cmd);

    // Update scene data for rendering
    void updateScene();

    // Pointer to the engine that owns this renderer
    VulkanEngine* _engine;
};