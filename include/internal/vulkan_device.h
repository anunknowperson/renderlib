
#pragma once

#include "GLFW/glfw3.h"

#include "vulkan/vulkan.h"
#include <vector>


#include "internal/vulkan_instance.h"
#include "internal/vulkan_physical_device.h"

class VulkanRender;

class VulkanDevice {

private:

    VulkanRender* render;

public:
    VkDevice device;

    // TODO: Consider moving queues to separate class(es)
    VkQueue graphicsQueue;
    VkQueue presentQueue;

	VulkanDevice(VulkanRender* p_render);
	~VulkanDevice();
};