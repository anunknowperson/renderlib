#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "vk_descriptors.h"
#include "vk_smart_wrappers.h"

constexpr unsigned int FRAME_OVERLAP = 2;

struct FrameData {
    std::unique_ptr<VulkanCommandPool> _commandPool;
    VkCommandBuffer _mainCommandBuffer;

    std::unique_ptr<VulkanSemaphore> _swapchainSemaphore, _renderSemaphore;
    std::unique_ptr<VulkanFence> _renderFence;

    DescriptorAllocatorGrowable _frameDescriptors;
    std::vector<std::unique_ptr<VulkanBuffer>> _frameBuffers; // For per-frame temporary buffers
};

class VulkanEngine;

class CommandBuffersContainer {
public:
    CommandBuffersContainer();
    ~CommandBuffersContainer() = default;

    // Frame management
    FrameData _frames[FRAME_OVERLAP];
    
    FrameData& get_current_frame(unsigned int frameNumber) {
        return _frames[frameNumber % FRAME_OVERLAP];
    }

    // Immediate command buffer members
    std::unique_ptr<VulkanFence> _immFence;
    VkCommandBuffer _immCommandBuffer;
    std::unique_ptr<VulkanCommandPool> _immCommandPool;

    // Initialization
    void init_sync_structures(VulkanEngine* vk_engine);

private:
    // No copying
    CommandBuffersContainer(const CommandBuffersContainer&) = delete;
    CommandBuffersContainer& operator=(const CommandBuffersContainer&) = delete;
};