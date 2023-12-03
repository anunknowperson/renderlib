
#pragma once



#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include <memory>

#include <vector>


#include "internal/vulkan_instance.h"
#include "internal/vulkan_physical_device.h"
#include "internal/vulkan_surface.h"
#include "vulkan_device.h"

class VulkanRender;

class VulkanSwapchain {
private:

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);


    VulkanRender* render;

public:
    VkSwapchainKHR swap_chain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;


    void createSwapChain();
    void createImageViews();
    void create_framebuffers();

    void cleanupSwapChain();

    void recreate();

    VulkanSwapchain(VulkanRender* p_render);
    ~VulkanSwapchain();
};
