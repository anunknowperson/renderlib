#include "internal/vulkan_command_pool.h"

#include "internal/vulkan_render.h"
#include "../core/logging.h"

VulkanCommandPool::VulkanCommandPool(VulkanRender *p_render) {
    render = p_render;

    VulkanPhysicalDevice::QueueFamilyIndices queueFamilyIndices = VulkanPhysicalDevice::findQueueFamilies(render->vulkan_physical_device, render->vulkan_surface->surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(render->vulkan_device->device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool.");
    }

    commandBuffers.resize(render->MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

    if (vkAllocateCommandBuffers(render->vulkan_device->device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers.");
    }


}

VulkanCommandPool::~VulkanCommandPool() {
    vkDestroyCommandPool(render->vulkan_device->device, commandPool, nullptr);
}

void VulkanCommandPool::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        LOGE("Failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = render->vulkan_render_pass->renderPass;
    renderPassInfo.framebuffer = render->vulkan_swap_chain->swapChainFramebuffers[imageIndex];

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = render->vulkan_swap_chain->swapChainExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, render->vulkan_graphics_pipeline->graphicsPipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(render->vulkan_swap_chain->swapChainExtent.width);
        viewport.height = static_cast<float>(render->vulkan_swap_chain->swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = render->vulkan_swap_chain->swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
        VkBuffer vertexBuffers[] = {render->vertexBuffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, render->indexBuffer, 0, VK_INDEX_TYPE_UINT16);

        // Bind your instance buffer with a different binding index
        VkBuffer instanceBuffers[] = {render->instanceBuffer};
        VkDeviceSize instanceOffsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 1, 1, instanceBuffers, instanceOffsets);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, render->vulkan_graphics_pipeline->pipelineLayout, 0, 1, &render->descriptorSets[render->currentFrame], 0, nullptr);

        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(render->indices.size()), render->instances.size(), 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        LOGE("Failed to record command buffer.");
    }
}
