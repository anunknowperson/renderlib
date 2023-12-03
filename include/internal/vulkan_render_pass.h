#pragma once

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include <memory>

#include <vector>


#include "internal/vulkan_instance.h"
#include "internal/vulkan_physical_device.h"
#include "internal/vulkan_surface.h"

class VulkanRender;

class VulkanRenderPass {
private:

    VulkanRender* render;
public:

    VkRenderPass renderPass;

    VulkanRenderPass(VulkanRender* p_render);
    ~VulkanRenderPass();
};