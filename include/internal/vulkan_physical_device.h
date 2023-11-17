
#pragma once

#include "GLFW/glfw3.h"

#include "vulkan/vulkan.h"
#include <vector>

#include <optional>


#include "internal/vulkan_instance.h"
#include <string>

// TODO: Get rid of consts
class VulkanPhysicalDevice {
public:
    // TODO: Remove this
    static const std::vector<const char*> deviceExtensions;

	static VkPhysicalDevice pick(const std::shared_ptr<VulkanInstance>& p_vulkan_instance, VkSurfaceKHR p_surface);

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        [[nodiscard]] bool isComplete() const;
    };

    static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR p_surface);

private:

	static void logDeviceDetails(VkPhysicalDevice device, uint32_t score);
	static int rateDeviceSuitability(VkPhysicalDevice device, VkSurfaceKHR p_surface);
    static bool checkDeviceExtensionSupport(VkPhysicalDevice device);
};