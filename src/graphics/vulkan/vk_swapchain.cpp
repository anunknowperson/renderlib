#include "graphics/vulkan/vk_swapchain.h"

#include <VkBootstrap.h>
#include "graphics/vulkan/vk_initializers.h"
#include "interfaces/ISwapchain.h"

namespace vk_swapchain {

ISwapchainController::Ptr make_swapchain_controller(std::shared_ptr<VulkanContext> vCtx, VmaAllocator allocator, SDL_Window* window) {
    return std::make_unique<SwapchainController>(std::move(vCtx), allocator, window);
}

} // namespace vk_swapchain

void SwapchainController::create_swapchain(uint32_t width, uint32_t height) {
    vkb::SwapchainBuilder swapchainBuilder{vCtxP->chosenGPU, vCtxP->device, vCtxP->surface};

    _swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

    auto swap_ret =
            swapchainBuilder
                    //.use_default_format_selection()
                    .set_desired_format(VkSurfaceFormatKHR{
                            .format = _swapchainImageFormat,
                            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
                    // use vsync present mode
                    .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                    .set_desired_extent(width, height)
                    .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                    .build();

    if (!swap_ret) {
        LOGE("Failed to create swapchain. Error: {}",
             swap_ret.error().message());
    }

    vkb::Swapchain vkbSwapchain = swap_ret.value();

    _swapchainExtent = vkbSwapchain.extent;
    // store swapchain and its related images
    _swapchain = vkbSwapchain.swapchain;
    _swapchainImages = vkbSwapchain.get_images().value();
    _swapchainImageViews = vkbSwapchain.get_image_views().value();
}

SwapchainController::SwapchainController(std::shared_ptr<VulkanContext> ctx,
                    VmaAllocator allocator,
                    SDL_Window* window)
    : vCtxP(std::move(ctx)),
      _allocator(allocator),
      _swapchainImageFormat(),
      _swapchainExtent(),
      _swapchain(nullptr),
      _window(window) {
    SwapchainController::create_swapchain(vCtxP->windowExtent.width, vCtxP->windowExtent.height);

    // draw image size will match the window
    VkExtent3D drawImageExtent = {vCtxP->windowExtent.width, vCtxP->windowExtent.height, 1};

    // hardcoding the draw format to 32-bit float
    vCtxP->drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    vCtxP->drawImage.imageExtent = drawImageExtent;
    VkImageUsageFlags drawImageUsages{};
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkImageCreateInfo rimg_info = vkinit::image_create_info(
            vCtxP->drawImage.imageFormat, drawImageUsages, drawImageExtent);
    // for the draw image, we want to allocate it from gpu local memory
    VmaAllocationCreateInfo rimg_allocinfo = {};
    rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    rimg_allocinfo.requiredFlags =
            VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // allocate and create the image
    vmaCreateImage(_allocator, &rimg_info, &rimg_allocinfo, &vCtxP->drawImage.image,
                   &vCtxP->drawImage.allocation, nullptr);
    // build an image-view for the draw image to use for rendering
    VkImageViewCreateInfo rview_info = vkinit::imageview_create_info(
            vCtxP->drawImage.imageFormat, vCtxP->drawImage.image,
            VK_IMAGE_ASPECT_COLOR_BIT);
    VK_CHECK(vkCreateImageView(vCtxP->device, &rview_info, nullptr,
                               &vCtxP->drawImage.imageView));
    // add to deletion queues
    vCtxP->mainDeletionQueue.push_function([=, this]() {
        vkDestroyImageView(vCtxP->device, vCtxP->drawImage.imageView, nullptr);
        vmaDestroyImage(_allocator, vCtxP->drawImage.image, vCtxP->drawImage.allocation);
    });
}

void SwapchainController::destroy_swapchain() {
    vkDestroySwapchainKHR(vCtxP->device, _swapchain, nullptr);

    // destroy swapchain resources
    for (auto & _swapchainImageView : _swapchainImageViews) {
        vkDestroyImageView(vCtxP->device, _swapchainImageView, nullptr);
    }
}

void SwapchainController::resize_swapchain() {
    vkDeviceWaitIdle(vCtxP->device);

    destroy_swapchain();

    int w, h;
    SDL_GetWindowSize(_window, &w, &h);
    vCtxP->windowExtent.width = static_cast<uint32_t>(w);
    vCtxP->windowExtent.height = static_cast<uint32_t>(h);

    create_swapchain(vCtxP->windowExtent.width, vCtxP->windowExtent.height);

    vCtxP->resize_requested = false;
}