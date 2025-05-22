#pragma once

#include <functional>
#include <vector>
#include <vulkan/vulkan.h>

class VulkanEngine;
struct FrameData;
struct DeletionQueue;

class CommandBuffers {
public:
    // Constructor initializes Vulkan command buffers and command pools.
    explicit CommandBuffers(VkDevice device, uint32_t graphicsQueueFamily,
                            VkQueue graphicsQueue, FrameData* frames,
                            DeletionQueue* deletionQueue, VulkanEngine& engine);

    // Immediately submit the command buffer with provided recording commands.
    // The passed `recordCommands` function is executed to record commands into
    // the buffer.
    void immediate_submit(
            std::function<void(VkCommandBuffer cmd)>&& recordCommands) const;

    // Initialize Vulkan command buffers and command pools for frame usage
    void init_commands();

private:
    VkCommandBuffer m_immCommandBuffer;
    VkCommandPool m_immCommandPool;
    // For synchronizing immediate submissions
    VkFence m_immFence;

    uint32_t m_graphicsQueueFamily;
    // Used to create resources
    VkDevice m_device;
    // Used for command buffer submission
    VkQueue m_graphicsQueue;
    // Pointer to frame data
    FrameData* m_frames;
    // Pointer for cleanup
    DeletionQueue* m_mainDeletionQueue;
    VulkanEngine& m_engine;
};