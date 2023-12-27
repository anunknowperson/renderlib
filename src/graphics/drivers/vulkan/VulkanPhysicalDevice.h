
#pragma once

#include <vector>
#include <string>
#include <optional>

#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"

#include "VulkanInstance.h"


class VulkanRender;
// TODO: Get rid of consts
class VulkanPhysicalDevice {
public:
    // TODO: Remove this
    static const std::vector<const char*> deviceExtensions;

	static VkPhysicalDevice pick(VulkanRender* p_render);

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        [[nodiscard]] bool isComplete() const;
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR p_surface);
    static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR p_surface);

private:

	static void logDeviceDetails(VkPhysicalDevice device, uint32_t score);
	static int rateDeviceSuitability(VkPhysicalDevice device, VkSurfaceKHR p_surface);
    static bool checkDeviceExtensionSupport(VkPhysicalDevice device);
};