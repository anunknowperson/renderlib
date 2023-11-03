#include "core/logging.h"
#include "internal/vulkan_physical_device.h"
#include <map>
#include <string>
#include <unordered_map>

namespace
{

const std::unordered_map<uint32_t, std::string_view> vendorNames =
{
    { 0x1002, "AMD" },
    { 0x1010, "ImgTec" },
    { 0x106B, "Apple" },
    { 0x10DE, "NVIDIA" },
    { 0x13B5, "ARM" },
    { 0x5143, "Qualcomm" },
    { 0x8086, "Intel" },
};
} // namespace

VkPhysicalDevice VulkanPhysicalDevice::pick(std::shared_ptr<VulkanInstance> p_instance)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(p_instance->instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        LOGE("Failed to find GPUs with Vulkan support.");
        return nullptr;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(p_instance->instance, &deviceCount, devices.data());

    std::multimap<int, VkPhysicalDevice> candidates;

    LOGI("Evaluating physical devices:");
    for (const auto& device : devices) {
        int score = rateDeviceSuitability(device);
        if (score > 0) { // Only consider devices that have a positive score
            candidates.insert(std::make_pair(score, device));
        }
        logDeviceDetails(device, score); // Log details about the device
    }

    if (candidates.empty()) {
        LOGE("Failed to find a suitable GPU.");
        return nullptr;
    }

    // The last element of the multimap has the highest rating
    VkPhysicalDevice selected_device = candidates.rbegin()->second;
    
    VkPhysicalDeviceProperties selected_device_properties;
    vkGetPhysicalDeviceProperties(selected_device, &selected_device_properties);
    LOGI("Selected device: {}", selected_device_properties.deviceName);

    return selected_device;
}

void VulkanPhysicalDevice::logDeviceDetails(VkPhysicalDevice device, int score) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    std::string deviceType;
    switch (deviceProperties.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: deviceType = "Integrated GPU"; break;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: deviceType = "Discrete GPU"; break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: deviceType = "Virtual GPU"; break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU: deviceType = "CPU"; break;
    default: deviceType = "Other"; break;
    }

    LOGIF("Device Name: {}", deviceProperties.deviceName);
    LOGIF("Device Type: {}", deviceType);
    LOGIF("API Version: {}.{}.{}",
        VK_VERSION_MAJOR(deviceProperties.apiVersion),
        VK_VERSION_MINOR(deviceProperties.apiVersion),
        VK_VERSION_PATCH(deviceProperties.apiVersion));
    LOGIF("Driver Version: {}", deviceProperties.driverVersion);
    LOGIF("Vendor ID: {}", deviceProperties.vendorID);
    
    std::string vendorName = "Unknown";
    auto it = vendorNames.find(deviceProperties.vendorID);
    if (it != vendorNames.end()) {
        vendorName = it->second;
    }

    LOGIF("Vendor Name: {}", vendorName);

    LOGIF("Device ID: {}", deviceProperties.deviceID);
    LOGIF("Geometry Shader Support: {}", deviceFeatures.geometryShader ? "Yes" : "No");
    
    LOGIF("Device score: {}", score);
}

int VulkanPhysicalDevice::rateDeviceSuitability(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    int rating = 0;

    // Use max image dimension as a basic metric for performance
    rating += deviceProperties.limits.maxImageDimension2D;

    // Geometry shader is a must-have feature
    if (!deviceFeatures.geometryShader) {
        return -1;
    }

    QueueFamilyIndices indices = findQueueFamilies(device);

    if (!indices.isComplete()) {
        return -1;
    }

    // Prefer discrete GPUs by multiplying the score by 2
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        rating *= 2;
    }

    // Add more criteria for rating as needed

    return rating;
}

VulkanPhysicalDevice::QueueFamilyIndices VulkanPhysicalDevice::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices{};
    
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}
