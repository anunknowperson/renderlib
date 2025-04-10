#include <cinttypes>

#include "graphics/vulkan/vk_command_buffers.h"
#include "graphics/vulkan/vk_engine.h"
#include "graphics/vulkan/vk_initializers.h"

CommandBuffers::CommandBuffers(VkDevice device, uint32_t graphicsQueueFamily,
                               VkQueue graphicsQueue, FrameData* frames,
                               DeletionQueue* deletionQueue,
                               VulkanEngine& engine)
    : m_graphicsQueueFamily(graphicsQueueFamily),
      m_device(device),
      m_graphicsQueue(graphicsQueue),
      m_frames(frames),
      m_mainDeletionQueue(deletionQueue),
      m_engine(engine) {
    const VkFenceCreateInfo fenceCreateInfo =
            vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
    VK_CHECK(vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_immFence));
}

void CommandBuffers::init_commands() {
    // create a command pool for commands submitted to the graphics queue.
    // we also want the pool to allow for resetting of individual command
    // buffers
    const VkCommandPoolCreateInfo commandPoolInfo =
            vkinit::command_pool_create_info(
                    m_graphicsQueueFamily,
                    VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    for (uint8_t frame_index = 0; frame_index < FRAME_OVERLAP; ++frame_index) {
        VK_CHECK(vkCreateCommandPool(m_device, &commandPoolInfo, nullptr,
                                     &m_frames[frame_index]._commandPool));

        // allocate the default command buffer that we will use for rendering
        VkCommandBufferAllocateInfo cmdAllocInfo =
                vkinit::command_buffer_allocate_info(
                        m_frames[frame_index]._commandPool,
                                                     1);

        VK_CHECK(vkAllocateCommandBuffers(m_device, &cmdAllocInfo,
                &m_frames[frame_index]._mainCommandBuffer));
    }

    VK_CHECK(vkCreateCommandPool(m_device, &commandPoolInfo, nullptr,
                                 &m_immCommandPool));

    // allocate the command buffer for immediate submits
    const VkCommandBufferAllocateInfo cmdAllocInfo =
            vkinit::command_buffer_allocate_info(m_immCommandPool, 1);

    VK_CHECK(vkAllocateCommandBuffers(m_device, &cmdAllocInfo,
                                      &m_immCommandBuffer));

    m_mainDeletionQueue->push_function([this] {
        vkDestroyCommandPool(m_device, m_immCommandPool, nullptr);
        vkDestroyFence(m_device, m_immFence, nullptr);
    });
}

void CommandBuffers::immediate_submit(
        std::function<void(VkCommandBuffer cmd)>&& recordCommands) const {
    VK_CHECK(vkResetFences(m_device, 1, &m_immFence));
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
    VK_CHECK(vkQueueSubmit2(m_graphicsQueue, 1, &submit, m_immFence));

    VK_CHECK(vkWaitForFences(m_device, 1, &m_immFence, true, 9999999999));
}