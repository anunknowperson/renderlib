#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <functional>

#include "vk_engine.h"

class CommandBuffers {
public:
    explicit CommandBuffersVkDevice device, FrameData* frames)
    : m_graphicsQueueFamily(graphicsQueueFamily), m_device(device), m_frames(frames){
        auto queue_family_ret =
                vkbDevice.get_queue_index(vkb::QueueType::graphics);
        if (!queue_family_ret) {
            LOGE("Failed to retrieve graphics queue family. Error: {}",
                 queue_family_ret.error().message());
        }

        _graphicsQueueFamily = queue_family_ret.value();
    }
    
    void immediate_submit(
            std::function<void(VkCommandBuffer cmd)>&& recordCommands) const;

    void init_commands();

private:
    VkCommandBuffer m_immCommandBuffer;
    VkCommandPool m_immCommandPool;

    std::unique_ptr<uint32_t> m_graphicsQueueFamily;
    VkDevice m_device;
    FrameData* m_frames;
};