
#pragma once

#include "GLFW/glfw3.h"

#include "vulkan/vulkan.h"
#include <vector>


#include "internal/vulkan_instance.h"
#include "internal/vulkan_physical_device.h"


class VulkanDevice {

private:


public:
    std::shared_ptr<VulkanInstance> vulkan_instance;
	VkPhysicalDevice physical_device;
    VkDevice device;

    // TODO: Consider moving queues to separate class(es)
    VkQueue graphicsQueue;
    VkQueue presentQueue;

	VulkanDevice(std::shared_ptr<VulkanInstance> p_vulkan_instance, VkPhysicalDevice p_physical_device, VkSurfaceKHR p_surface);
	~VulkanDevice();
};