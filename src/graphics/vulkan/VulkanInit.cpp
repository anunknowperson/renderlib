#include "graphics/vulkan/VulkanInit.h"

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan_core.h>

#include "core/Logging.h"
#include "graphics/vulkan/Device.h"

namespace vkb {
struct Device;
}

#ifdef NDEBUG
constexpr bool bUseValidationLayers = false;
#else
constexpr bool bUseValidationLayers = true;
#endif

VulkanInit::Window::Window() : window{SDL_CreateWindow("engine", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, 1700, 900, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE)} {}
VulkanInit::Window::~Window() { SDL_DestroyWindow(window); }

VulkanInit::Surface::Surface(SDL_Window *window, const VkInstance& instance) {
    const SDL_bool err = SDL_Vulkan_CreateSurface(window, instance, &surface);
    if (err == SDL_FALSE) {
        LOGE("Failed to create Vulkan surface. Error: {}", SDL_GetError());
    }
    _instance = instance;
}

VulkanInit::Surface::~Surface() {
    vkDestroySurfaceKHR(_instance, surface, nullptr);
}

VulkanInit::Queue::Queue(const vkb::Device& dv)
    : queue([&] {
          auto res = dv.get_queue(vkb::QueueType::graphics);
          if (!res) {
              LOGE("Failed to retrieve graphics queue. Error: {}",
                   res.error().message());
          }
            return res.value();
      }()),
      index([&] {
          auto queue_family_ret = dv.get_queue_index(vkb::QueueType::graphics);
          if (!queue_family_ret) {
              LOGE("Failed to retrieve graphics queue family. Error: {}",
                   queue_family_ret.error().message());
          }

          return queue_family_ret.value();
      }()) {}

VulkanInit::VulkanInit()
    : surface(window.window, getInstance())
    , device(instance.instance, getSurface())
    , queue(device.logical)
    , allocator(getPhysicalDevice(), device.logical, getInstance()){}

VulkanInit::Instance::Instance() : instance([] {
    vkb::InstanceBuilder builder;

    auto system_info_ret = vkb::SystemInfo::get_system_info();
    if (!system_info_ret) {
        LOGE("Failed to retrieve system info. Error: {}",
             system_info_ret.error().message());
    }

    auto system_info = system_info_ret.value();

    LOGI("Available layers:")

    for (auto& layer : system_info.available_layers) {
        LOGI(layer.layerName);
    }

    LOGI("Available extensions:")

    for (auto& [extensionName, _] : system_info.available_extensions) {
        LOGI(extensionName);
    }

    auto inst_ret = builder.set_app_name("TODO: PUT APP NAME HERE")
                            .set_engine_name("rainsystem")
                            .request_validation_layers(bUseValidationLayers)
                            .set_debug_callback(debugCallback)
                            .require_api_version(1, 3, 0)
                            .build();
    if (!inst_ret) {
        LOGE("Failed to create Vulkan instance. Error: {}",
             inst_ret.error().message());
    }
    return inst_ret.value();
}()){}

VulkanInit::Instance::~Instance() {
    vkb::destroy_instance(instance);
}

VulkanInit::Allocator::Allocator(const VkPhysicalDevice& pGpu,
                              const VkDevice& pDevice,
                              const VkInstance& pInstance) {
    const VmaAllocatorCreateInfo info = {
        .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
        .physicalDevice = pGpu,
        .device = pDevice,
        .instance = pInstance
    };
    vmaCreateAllocator(&info, &allocator);
}

VulkanInit::Allocator::~Allocator() {
    vmaDestroyAllocator(allocator);
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanInit::debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        [[maybe_unused]] void* pUserData) {
    std::string type;

    switch (messageType) {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
            type = "General";

            break;

        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
            type = "Validation";

            break;

        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
            type = "Performance";

            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
            type = "Modified set of GPU-visible virtual addresses";
            break;
        default:
            type = "Unknown";
    }

    std::string message = "(" + type + ")" + pCallbackData->pMessage;

    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        LOGD(message)
    } else if (messageSeverity ==
               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        LOGI(message)
    } else if (messageSeverity ==
               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        LOGW(message)
    } else if (messageSeverity >=
               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        LOGE("{}", message)
    } else {
        LOGE("{}", message)
    }

    return VK_FALSE;
}