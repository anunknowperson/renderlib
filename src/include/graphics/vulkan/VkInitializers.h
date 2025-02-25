#pragma once

#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace vkinit {
//> init_cmd
VkCommandPoolCreateInfo commandPoolCreateInfo(
        uint32_t queue_family_index, VkCommandPoolCreateFlags flags = 0);
VkCommandBufferAllocateInfo commandBufferAllocateInfo(VkCommandPool pool,
                                                         uint32_t count = 1);
//< init_cmd

VkCommandBufferBeginInfo commandBufferBeginInfo(
        VkCommandBufferUsageFlags flags = 0);
VkCommandBufferSubmitInfo commandBufferSubmitInfo(VkCommandBuffer cmd);

VkFenceCreateInfo fenceCreateInfo(VkFenceCreateFlags flags = 0);

VkSemaphoreCreateInfo semaphoreCreateInfo(VkSemaphoreCreateFlags flags = 0);

VkSubmitInfo2 submitInfo(const VkCommandBufferSubmitInfo* cmd,
                          const VkSemaphoreSubmitInfo* signal_semaphore_info,
                          const VkSemaphoreSubmitInfo* wait_semaphore_info);
VkPresentInfoKHR presentInfo();

VkRenderingAttachmentInfo attachmentInfo(
        VkImageView view, const VkClearValue* clear,
        VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/);

VkRenderingAttachmentInfo depthAttachmentInfo(
        VkImageView view,
        VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/);

VkRenderingInfo renderingInfo(
        VkExtent2D render_extent,
        const VkRenderingAttachmentInfo* color_attachment,
        const VkRenderingAttachmentInfo* depth_attachment);

VkImageSubresourceRange imageSubresourceRange(VkImageAspectFlags aspect_mask);

VkSemaphoreSubmitInfo semaphoreSubmitInfo(VkPipelineStageFlags2 stage_mask,
                                            VkSemaphore semaphore);
VkDescriptorSetLayoutBinding descriptorSetLayoutBinding(
        VkDescriptorType type, VkShaderStageFlags stage_flags, uint32_t binding);
VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo(
        const VkDescriptorSetLayoutBinding* bindings, uint32_t binding_count);
VkWriteDescriptorSet writeDescriptorImage(
        VkDescriptorType type, VkDescriptorSet dst_set,
        const VkDescriptorImageInfo* image_info, uint32_t binding);
VkWriteDescriptorSet writeDescriptorBuffer(
        VkDescriptorType type, VkDescriptorSet dst_set,
        const VkDescriptorBufferInfo* buffer_info, uint32_t binding);
VkDescriptorBufferInfo bufferInfo(VkBuffer buffer, VkDeviceSize offset,
                                   VkDeviceSize range);

VkImageCreateInfo imageCreateInfo(VkFormat format,
                                    VkImageUsageFlags usage_flags,
                                    VkExtent3D extent);
VkImageViewCreateInfo imageviewCreateInfo(VkFormat format, VkImage image,
                                            VkImageAspectFlags aspect_flags);
VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo();
VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(
        VkShaderStageFlagBits stage, VkShaderModule shader_module,
        const char* entry = "main");
}  // namespace vkinit
