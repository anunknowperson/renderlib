#include "vk_command_buffers.h"

CommandBufferManager::CommandBufferManager(VkDevice device,
                                           uint32_t queueFamilyIndex)
    : _device(device),
      _queueFamilyIndex(queueFamilyIndex),
      _commandPool(VK_NULL_HANDLE) {}

CommandBufferManager::~CommandBufferManager() {
    cleanup();
}

void CommandBufferManager::initCommandBuffers(size_t frameCount) {
    VkCommandPoolCreateInfo poolInfo = vkinit::command_pool_create_info(
            _queueFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    VK_CHECK(vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool));

    _commandBuffers.resize(frameCount);
    VkCommandBufferAllocateInfo allocInfo =
            vkinit::command_buffer_allocate_info(_commandPool, frameCount);
    VK_CHECK(vkAllocateCommandBuffers(_device, &allocInfo,
                                      _commandBuffers.data()));
}

VkCommandBuffer CommandBufferManager::beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo =
            vkinit::command_buffer_allocate_info(_commandPool, 1);
    VkCommandBuffer commandBuffer;
    VK_CHECK(vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer));

    VkCommandBufferBeginInfo beginInfo = vkinit::command_buffer_begin_info(
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    return commandBuffer;
}

void CommandBufferManager::endSingleTimeCommands(VkCommandBuffer commandBuffer,
                                                 VkQueue queue) {
    VK_CHECK(vkEndCommandBuffer(commandBuffer));

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
    VK_CHECK(vkQueueWaitIdle(queue));

    vkFreeCommandBuffers(_device, _commandPool, 1, &commandBuffer);
}

VkCommandBuffer CommandBufferManager::getCommandBuffer(
        size_t frameIndex) const {
    return _commandBuffers[frameIndex];
}

void CommandBufferManager::cleanup() {
    if (_commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(_device, _commandPool, nullptr);
        _commandPool = VK_NULL_HANDLE;
    }
}
