#pragma once
#include <SDL2/SDL_video.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

#include "Device.h"

struct VulkanInit {
    VulkanInit();
    VkDevice getLogicalDevice() const { return device.logical; }
    VkPhysicalDevice getPhysicalDevice() const { return device.physical; }
    VkQueue getQueue() const { return queue.queue; }
    uint32_t getQueueIndex() const { return queue.index; }
    VmaAllocator getAllocator() const { return allocator.allocator; }
    VkInstance getInstance() const { return instance.instance; }
    VkSurfaceKHR getSurface() const { return surface.surface; }
    SDL_Window* getWindow() const { return window.window; }
private:
    struct Instance {
        vkb::Instance instance;
        Instance();
        ~Instance();
    };
    Instance instance;

    struct Window {
        SDL_Window* window;
        Window();
        ~Window();
    };
    Window window;

    struct Surface {
        VkSurfaceKHR surface{VK_NULL_HANDLE};        // Vulkan window surface
        explicit Surface(SDL_Window *window, const VkInstance& instance);
        ~Surface();
    private:
        VkInstance _instance{VK_NULL_HANDLE};
    };
    Surface surface;

    Device device;

    struct Queue {
        VkQueue queue;
        uint32_t index;
        operator VkQueue() const { return queue; }
        Queue(const vkb::Device& dv);
    };
    Queue queue;

    struct Allocator {
        VmaAllocator allocator{VK_NULL_HANDLE};
        Allocator(const VkPhysicalDevice& gpu, const VkDevice& device, const VkInstance& instance);
        ~Allocator();
    };
    Allocator allocator;

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
              void* pUserData);
};
