#pragma once
#include <VkBootstrap.h>
#include <vulkan/vulkan_core.h>

struct Device {
    explicit operator VkDevice() const;
    explicit operator vkb::Device() const;
    void init(const vkb::PhysicalDevice& physical_device);
    ~Device();

private:
    vkb::Device _device;
};
