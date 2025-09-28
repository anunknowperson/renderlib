#include "graphics/vulkan/Device.h"

#include "core/Logging.h"

void Device::init(const vkb::PhysicalDevice& physical_device) {
    const vkb::DeviceBuilder device_builder{physical_device};
    auto dev_ret = device_builder.build();
    if (!dev_ret) {
        LOGE("Failed to create logical device. Error: {}",
             dev_ret.error().message());
    }
    _device = dev_ret.value();
}

Device::~Device() {
    vkb::destroy_device(_device);
}

Device::operator VkDevice() const {
    return _device;
}

Device::operator vkb::Device() const {
    return _device;
}