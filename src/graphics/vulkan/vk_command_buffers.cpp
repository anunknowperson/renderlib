#include "graphics/vulkan/vk_command_buffers.h"
#include "graphics/vulkan/vk_engine.h"
#include "graphics/vulkan/vk_initializers.h"

void CommandBuffers::init_commands(VulkanEngine* vk_engine) {
    // create a command pool for commands submitted to the graphics queue.
    // we also want the pool to allow for resetting of individual command
    // buffers
    const VkCommandPoolCreateInfo commandPoolInfo =
            vkinit::command_pool_create_info(
                    vk_engine->_graphicsQueueFamily,
                    VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    for (auto& _frame : vk_engine->command_buffers_container._frames) {
        VkCommandPool commandPool;
        VK_CHECK(vkCreateCommandPool(vk_engine->_device, &commandPoolInfo,
                                     nullptr,
                                     &commandPool));

        _frame._commandPool = std::make_unique<VulkanCommandPool>(vk_engine->_device, commandPool);

        // allocate the default command buffer that we will use for rendering
        VkCommandBufferAllocateInfo cmdAllocInfo =
                vkinit::command_buffer_allocate_info(_frame._commandPool->get(), 1);

        VK_CHECK(vkAllocateCommandBuffers(vk_engine->_device, &cmdAllocInfo,
                                          &_frame._mainCommandBuffer));
    }

    VkCommandPool immCommandPool;
    VK_CHECK(vkCreateCommandPool(vk_engine->_device, &commandPoolInfo, nullptr,
                                 &immCommandPool));

    vk_engine->command_buffers_container._immCommandPool = std::make_unique<VulkanCommandPool>(vk_engine->_device, immCommandPool);

    // allocate the command buffer for immediate submits
    const VkCommandBufferAllocateInfo cmdAllocInfo =
            vkinit::command_buffer_allocate_info(vk_engine->command_buffers_container._immCommandPool->get(), 1);

    VK_CHECK(vkAllocateCommandBuffers(vk_engine->_device, &cmdAllocInfo,
                                      &(vk_engine->command_buffers_container._immCommandBuffer)));

    // Smart pointer will automatically handle cleanup - no need for deletion queue
}

void CommandBuffers::immediate_submit(
        std::function<void(VkCommandBuffer cmd)>&& function,
        VulkanEngine* vk_engine) const {
    VK_CHECK(vkResetFences(vk_engine->_device, 1, vk_engine->command_buffers_container._immFence->getPtr()));
    VK_CHECK(vkResetCommandBuffer(vk_engine->command_buffers_container._immCommandBuffer, 0));

    const VkCommandBuffer cmd = vk_engine->command_buffers_container._immCommandBuffer;

    const VkCommandBufferBeginInfo cmdBeginInfo =
            vkinit::command_buffer_begin_info(
                    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    function(cmd);

    VK_CHECK(vkEndCommandBuffer(cmd));

    const VkCommandBufferSubmitInfo cmdinfo =
            vkinit::command_buffer_submit_info(cmd);
    const VkSubmitInfo2 submit =
            vkinit::submit_info(&cmdinfo, nullptr, nullptr);

    // submit command buffer to the queue and execute it.
    //  _renderFence will now block until the graphic commands finish execution
    VK_CHECK(vkQueueSubmit2(vk_engine->_graphicsQueue, 1, &submit,
                            vk_engine->command_buffers_container._immFence->get()));

    VK_CHECK(vkWaitForFences(vk_engine->_device, 1, vk_engine->command_buffers_container._immFence->getPtr(), true,
                             9999999999));
}
