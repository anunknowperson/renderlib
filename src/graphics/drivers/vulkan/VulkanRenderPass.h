#pragma once

#include <memory>
#include <vector>

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"

class VulkanRender;

class VulkanRenderPass {
public:
    VulkanRenderPass(VulkanRender* p_render);
    ~VulkanRenderPass();

    VkRenderPass renderPass;


private:

    VulkanRender* render;

};