#pragma once

#include "graphics/vulkan/vk_types.h"

/*! \brief
 *
 * Interface for managing Vulkan swapchain operations
 * Provides control over swapchain creation, destruction, and resizing,
 * as well as access to swapchain images and image views
 */
class ISwapchainController {
public:
    virtual ~ISwapchainController() = default;

     /*! \brief
     * Creates a new swapchain with specified dimensions
     * @param width The width of the swapchain images
     * @param height The height of the swapchain images
     */
    virtual void create_swapchain(uint32_t width, uint32_t height) = 0;

    /*! \brief
     * Destroys the current swapchain and releases associated resources
     */
    virtual void destroy_swapchain() = 0;

    /*! \brief
     * Handles swapchain resizing (typically called when window is resized)
     */
    virtual void resize_swapchain() = 0;

    /*! \brief
     * Gets the image format used by the swapchain
     * @return Pointer to the VkFormat of swapchain images
     */
    virtual const VkFormat* get_swapchain_image_format() const = 0;

    /*! \brief
     * Gets the image format used by the swapchain
     * @return Pointer to the VkFormat of swapchain images
     */
    virtual VkExtent2D get_swapchain_extent() const = 0;

    /*! \brief
     * Gets the Vulkan swapchain handle
     * @return The VkSwapchainKHR handle
     */
    virtual VkSwapchainKHR get_swapchain() const = 0;

    /*! \brief
     * Gets a pointer to the Vulkan swapchain handle
     * @return Pointer to the VkSwapchainKHR handle
     */
    virtual const VkSwapchainKHR* get_swapchain_ptr() const = 0;

    /*! \brief
     * Gets all images in the swapchain
     * @return Vector of VkImage handles
     */
    virtual std::vector<VkImage> get_swapchain_images() const = 0;

    /*! \brief
     * Gets a specific swapchain image by index
     * @param i Index of the image to retrieve
     * @return VkImage handle for the requested image
     */
    virtual VkImage get_swapchain_image_by_index(const uint32_t i) const = 0;

    /*! \brief
     * Gets a specific swapchain image view by index
     * @param i Index of the image view to retrieve
     * @return VkImageView handle for the requested image view
     */
    virtual VkImageView get_swapchain_image_view_by_index(const uint32_t i) const = 0;

    /*! \brief
     * Gets all image views in the swapchain
     * @return Vector of VkImageView handles
     */
    virtual std::vector<VkImageView> get_swapchain_image_views() const = 0;

    using Ptr = std::unique_ptr<ISwapchainController>;
};
