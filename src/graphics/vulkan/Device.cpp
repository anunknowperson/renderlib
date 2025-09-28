#include "graphics/vulkan/Device.h"

#include "core/Logging.h"

Device::Device(const vkb::Instance& instance, const VkSurfaceKHR surface)
    : physical(VK_NULL_HANDLE), logical([&] {
          VkPhysicalDeviceVulkan13Features features{
                  .synchronization2 = true,
                  .dynamicRendering = true,
          };

          VkPhysicalDeviceVulkan12Features features12{
                  .descriptorIndexing = true,
                  .bufferDeviceAddress = true,
          };

          // use vkbootstrap to select a gpu.
          // We want a gpu that can write to the SDL surface and supports
          // vulkan 1.3 with the correct features
          vkb::PhysicalDeviceSelector selector{instance};

          auto res = selector.set_minimum_version(1, 3)
                             .set_required_features_13(features)
                             .set_required_features_12(features12)
                             .set_surface(surface)
                             .select();
          if (!res) {
              LOGE("Failed to select physical device. Error: {}",
                   res.error().message());
          }

          const vkb::PhysicalDevice& physical_device = res.value();

          const vkb::DeviceBuilder device_builder{physical_device};
          auto dv_res = device_builder.build();
          if (!dv_res) {
              LOGE("Failed to create logical device. Error: {}",
                   dv_res.error().message());
          }
          physical = physical_device.physical_device;
          return dv_res.value();
      }()) {}

Device::~Device() {
    vkb::destroy_device(logical);
}
