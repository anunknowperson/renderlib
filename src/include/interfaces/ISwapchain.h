#pragma once

#include "graphics/vulkan/vk_types.h"

class ISwapchainController {
public:
    virtual ~ISwapchainController() = default;
    virtual void create_swapchain(uint32_t width, uint32_t height) = 0;
    virtual void destroy_swapchain() = 0;
    virtual void resize_swapchain() = 0;

    virtual const VkFormat* get_swapchain_image_format() const = 0;
    virtual VkExtent2D get_swapchain_extent() const = 0;
    virtual VkSwapchainKHR get_swapchain() const = 0;
    virtual const VkSwapchainKHR* get_swapchain_ptr() const = 0;
    virtual std::vector<VkImage> get_swapchain_images() const = 0;
    virtual VkImage get_swapchain_image_by_index(const uint32_t i) const = 0;
    virtual VkImageView get_swapchain_image_view_by_index(const uint32_t i) const = 0;
    virtual std::vector<VkImageView> get_swapchain_image_views() const = 0;

    using SwapchainPtr = std::unique_ptr<ISwapchainController>;
};
