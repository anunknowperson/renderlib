#pragma once
#include <VkBootstrap.h>
#include <vulkan/vulkan_core.h>

struct Device {
    VkPhysicalDevice physical;
    vkb::Device logical;
    Device(const vkb::Instance &instance, VkSurfaceKHR surface);
    ~Device();
};
