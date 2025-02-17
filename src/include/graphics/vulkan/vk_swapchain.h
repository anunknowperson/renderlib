#pragma once

#include "SDL2/SDL_vulkan.h"
#include "interfaces/ISwapchain.h"
#include "vk_types.h"

class SwapchainController : public ISwapchainController {
public:
    SwapchainController(std::shared_ptr<VulkanContext> ctx,
                        VmaAllocator allocator, SDL_Window* window);

    void create_swapchain(uint32_t width, uint32_t height) override;
    void destroy_swapchain() override;
    void resize_swapchain() override;

    const VkFormat* get_swapchain_image_format() const override {
        return &_swapchainImageFormat;
    }

    VkExtent2D get_swapchain_extent() const override {
        return _swapchainExtent;
    }

    VkSwapchainKHR get_swapchain() const override {
        return _swapchain;
    }

    const VkSwapchainKHR* get_swapchain_ptr() const override {
        return &_swapchain;
    }

    std::vector<VkImage> get_swapchain_images() const override {
        return _swapchainImages;
    }

    VkImage get_swapchain_image_by_index(
            const uint32_t i) const override {
        return _swapchainImages[i];
    }

    VkImageView get_swapchain_image_view_by_index(
            const uint32_t i) const override {
        return _swapchainImageViews[i];
    }

    std::vector<VkImageView> get_swapchain_image_views() const override {
        return _swapchainImageViews;
    }

private:
    std::shared_ptr<VulkanContext> vCtx;

    VmaAllocator _allocator;

    VkFormat _swapchainImageFormat;
    VkExtent2D _swapchainExtent;
    VkSwapchainKHR _swapchain;
    std::vector<VkImage> _swapchainImages;
    std::vector<VkImageView> _swapchainImageViews;

    SDL_Window* _window{nullptr};
};

namespace vk_swapchain {

SwapchainController::Ptr make_swapchain_controller(
        std::shared_ptr<VulkanContext> vCtx, VmaAllocator allocator,
        SDL_Window* window);

}  // namespace vk_swapchain