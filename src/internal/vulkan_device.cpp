#include "core/logging.h"

#include "internal/vulkan_device.h"

#include <set>



VulkanDevice::VulkanDevice(std::shared_ptr<VulkanInstance> p_vulkan_instance, VkPhysicalDevice p_physical_device, VkSurfaceKHR p_surface)
{
    vulkan_instance = p_vulkan_instance;
    physical_device = p_physical_device;

    VulkanPhysicalDevice::QueueFamilyIndices indices = VulkanPhysicalDevice::findQueueFamilies(physical_device, p_surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(VulkanPhysicalDevice::deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = VulkanPhysicalDevice::deviceExtensions.data();

    if (vulkan_instance->enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(vulkan_instance->validationLayers.size());
        createInfo.ppEnabledLayerNames = vulkan_instance->validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physical_device, &createInfo, nullptr, &device) != VK_SUCCESS) {
        LOGE("Failed to create vulkan logical device.");
    }

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

VulkanDevice::~VulkanDevice()
{
    vkDestroyDevice(device, nullptr);
}
