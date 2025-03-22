#include "graphics/vulkan/vk_command_buffers.h"
#include "graphics/vulkan/vk_engine.h"
#include "graphics/vulkan/vk_initializers.h"

void CommandBuffers::init_commands(VulkanEngine& vk_engine) {
    // create a command pool for commands submitted to the graphics queue.
    // we also want the pool to allow for resetting of individual command
    // buffers
    const VkCommandPoolCreateInfo commandPoolInfo =
            vkinit::command_pool_create_info(
                    vk_engine._graphicsQueueFamily,
                    VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    for (auto& _frame : vk_engine._frames) {
        VK_CHECK(vkCreateCommandPool(vk_engine._device, &commandPoolInfo,
                                     nullptr,
                                     &_frame._commandPool));

        // allocate the default command buffer that we will use for rendering
        VkCommandBufferAllocateInfo cmdAllocInfo =
                vkinit::command_buffer_allocate_info(_frame._commandPool, 1);

        VK_CHECK(vkAllocateCommandBuffers(vk_engine._device, &cmdAllocInfo,
                                          &_frame._mainCommandBuffer));
    }

    VK_CHECK(vkCreateCommandPool(vk_engine._device, &commandPoolInfo, nullptr,
                                 &(m_immCommandPool)));

    // allocate the command buffer for immediate submits
    const VkCommandBufferAllocateInfo cmdAllocInfo =
            vkinit::command_buffer_allocate_info(m_immCommandPool, 1);

    VK_CHECK(vkAllocateCommandBuffers(vk_engine._device, &cmdAllocInfo,
                                      &(m_immCommandBuffer)));

    vk_engine._mainDeletionQueue.push_function([=] {
        vkDestroyCommandPool(vk_engine._device, m_immCommandPool,
                             nullptr);
    });
}

void CommandBuffers::immediate_submit(
        std::function<void(VkCommandBuffer cmd)>&& recordCommands,
        VulkanEngine& vk_engine) const {
    VK_CHECK(vkResetFences(vk_engine._device, 1, &(vk_engine._immFence)));
    VK_CHECK(vkResetCommandBuffer(m_immCommandBuffer, 0));

    const VkCommandBuffer cmd = m_immCommandBuffer;

    const VkCommandBufferBeginInfo cmdBeginInfo =
            vkinit::command_buffer_begin_info(
                    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    recordCommands(cmd);

    VK_CHECK(vkEndCommandBuffer(cmd));

    const VkCommandBufferSubmitInfo cmdinfo =
            vkinit::command_buffer_submit_info(cmd);
    const VkSubmitInfo2 submit =
            vkinit::submit_info(&cmdinfo, nullptr, nullptr);

    // submit command buffer to the queue and execute it.
    //  _renderFence will now block until the graphic commands finish execution
    VK_CHECK(vkQueueSubmit2(vk_engine._graphicsQueue, 1, &submit,
                            vk_engine._immFence));

    VK_CHECK(vkWaitForFences(vk_engine._device, 1, &(vk_engine._immFence), true,
                             9999999999));
}