
#pragma once

#include "vulkan/vulkan.h"
#include <vector>

#include "GLFW/glfw3.h"

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"

class VulkanRender;

class VulkanDevice {

public:
    VulkanDevice(VulkanRender* p_render);
    ~VulkanDevice();

    VkDevice device;

    // TODO: Consider moving queues to separate class(es)
    VkQueue graphicsQueue;
    VkQueue presentQueue;

private:

    VulkanRender* render;


};