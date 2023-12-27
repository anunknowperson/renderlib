
#pragma once

#include <memory>
#include <vector>

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"

class VulkanRender;

class VulkanSwapChain {
public:
    VulkanSwapChain(VulkanRender* p_render);
    ~VulkanSwapChain();

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



private:

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);


    VulkanRender* render;


};
