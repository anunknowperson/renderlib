
#pragma once

#include "GLFW/glfw3.h"

#include "vulkan/vulkan.h"
#include <vector>

#include <optional>


#include "internal/vulkan_instance.h"
#include <string>


class VulkanPhysicalDevice {
public:
	static VkPhysicalDevice pick(std::shared_ptr<VulkanInstance> p_vulkan_instance);
	

private:
	std::shared_ptr<VulkanInstance> vulkan_instance;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;


	static void logDeviceDetails(VkPhysicalDevice device, int score);
	static int rateDeviceSuitability(VkPhysicalDevice device);

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;

		bool isComplete() {
			return graphicsFamily.has_value();
		}
	};

	static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

};