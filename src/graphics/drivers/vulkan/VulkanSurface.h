#pragma once

#include <memory>
#include <vector>

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"

class VulkanRender;

class VulkanSurface {
public:
    VulkanSurface(VulkanRender* p_render);
    ~VulkanSurface();

    VkSurfaceKHR surface;


private:

    VulkanRender* render;


};
