#include "graphics/vulkan/vk_command_buffers_container.h"
#include "graphics/vulkan/vk_engine.h"
#include "graphics/vulkan/vk_initializers.h"

CommandBuffersContainer::CommandBuffersContainer() {
    // Initialize members - actual initialization happens in init_sync_structures
    _immCommandBuffer = VK_NULL_HANDLE;
}

void CommandBuffersContainer::init_sync_structures(VulkanEngine* vk_engine) {
    const VkFenceCreateInfo fenceCreateInfo =
            vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
    const VkSemaphoreCreateInfo semaphoreCreateInfo =
            vkinit::semaphore_create_info();

    for (auto& _frame : _frames) {
        VkFence renderFence;
        VK_CHECK(vkCreateFence(vk_engine->_device, &fenceCreateInfo, nullptr, &renderFence));
        _frame._renderFence = std::make_unique<VulkanFence>(vk_engine->_device, renderFence);

        VkSemaphore swapchainSemaphore, renderSemaphore;
        VK_CHECK(vkCreateSemaphore(vk_engine->_device, &semaphoreCreateInfo, nullptr, &swapchainSemaphore));
        VK_CHECK(vkCreateSemaphore(vk_engine->_device, &semaphoreCreateInfo, nullptr, &renderSemaphore));
        
        _frame._swapchainSemaphore = std::make_unique<VulkanSemaphore>(vk_engine->_device, swapchainSemaphore);
        _frame._renderSemaphore = std::make_unique<VulkanSemaphore>(vk_engine->_device, renderSemaphore);
    }

    VkFence immFence;
    VK_CHECK(vkCreateFence(vk_engine->_device, &fenceCreateInfo, nullptr, &immFence));
    _immFence = std::make_unique<VulkanFence>(vk_engine->_device, immFence);
}