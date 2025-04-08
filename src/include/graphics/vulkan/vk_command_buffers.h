#pragma once

#include <functional>
#include <vector>
#include <vulkan/vulkan.h>

class VulkanEngine;
struct FrameData;
struct DeletionQueue;

class CommandBuffers {
public:
    explicit CommandBuffers(VkDevice device, uint32_t graphicsQueueFamily,
                            VkQueue graphicsQueue, FrameData* frames,
                            DeletionQueue* deletionQueue, VulkanEngine& engine);

    void immediate_submit(
            std::function<void(VkCommandBuffer cmd)>&& recordCommands) const;

    void init_commands();

private:
    VkCommandBuffer m_immCommandBuffer;
    VkCommandPool m_immCommandPool;
    VkFence m_immFence;

    uint32_t m_graphicsQueueFamily;
    VkDevice m_device;
    VkQueue m_graphicsQueue;
    FrameData* m_frames;
    DeletionQueue* m_mainDeletionQueue;
    VulkanEngine& m_engine;
};