#include "graphics/vulkan/vk_command_buffers.h"
#include "graphics/vulkan/vk_engine.h"
#include "graphics/vulkan/vk_initializers.h"





void CommandBuffers::init_commands() {
    // create a command pool for commands submitted to the graphics queue.
    // we also want the pool to allow for resetting of individual command
    // buffers
    const VkCommandPoolCreateInfo commandPoolInfo =
            vkinit::command_pool_create_info(
                    _graphicsQueueFamily,
                    VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    for (auto& _frame : _frames) {
        VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr,
                                     &_frame._commandPool));

        // allocate the default command buffer that we will use for rendering
        VkCommandBufferAllocateInfo cmdAllocInfo =
                vkinit::command_buffer_allocate_info(_frame._commandPool, 1);

        VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo,
                                          &_frame._mainCommandBuffer));
    }

    VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr,
                                 &_immCommandPool));

    // allocate the command buffer for immediate submits
    const VkCommandBufferAllocateInfo cmdAllocInfo =
            vkinit::command_buffer_allocate_info(_immCommandPool, 1);

    VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo,
                                      &_immCommandBuffer));

    _mainDeletionQueue.push_function([=, this] {
        vkDestroyCommandPool(_device, _immCommandPool, nullptr);
    });
}

void CommandBuffers::immediate_submit(
        std::function<void(VkCommandBuffer cmd)>&& function) const {
    VK_CHECK(vkResetFences(_device, 1, &_immFence));
    VK_CHECK(vkResetCommandBuffer(_immCommandBuffer, 0));

    const VkCommandBuffer cmd = _immCommandBuffer;

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
    VK_CHECK(vkQueueSubmit2(_graphicsQueue, 1, &submit, _immFence));

    VK_CHECK(vkWaitForFences(_device, 1, &_immFence, true, 9999999999));
}