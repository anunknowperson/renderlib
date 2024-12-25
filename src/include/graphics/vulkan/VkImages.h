#pragma once

#include <vulkan/vulkan.h>

namespace vkutil {

void transitionImage(VkCommandBuffer cmd, VkImage image,
                      VkImageLayout current_layout, VkImageLayout new_layout);

void copyImageToImage(VkCommandBuffer cmd, VkImage source,
                         VkImage destination, VkExtent2D src_size,
                         VkExtent2D dst_size);
};  // namespace vkutil