#pragma once


#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include <memory>

#include <vector>


#include "internal/vulkan_instance.h"
#include "internal/vulkan_physical_device.h"

class VulkanRender;

class VulkanSurface {
private:

    VulkanRender* render;

public:
    VkSurfaceKHR surface;

    VulkanSurface(VulkanRender* p_render);
    ~VulkanSurface();
};
