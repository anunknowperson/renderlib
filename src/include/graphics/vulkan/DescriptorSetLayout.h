#pragma once
#include <initializer_list>
#include <utility>
#include <vulkan/vulkan_core.h>

struct DescriptorSetLayout {
    VkDescriptorSetLayout set{VK_NULL_HANDLE};
    VkDevice device{VK_NULL_HANDLE};
    void create(VkDevice pDevice, VkShaderStageFlags pShaderStages,
                std::initializer_list<std::pair<uint32_t, VkDescriptorType>>
                        bindings);

    ~DescriptorSetLayout();
};