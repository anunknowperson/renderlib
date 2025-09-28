#include "graphics/vulkan/DescriptorSetLayout.h"

#include "graphics/vulkan/vk_descriptors.h"

void DescriptorSetLayout::create(
        const VkDevice pDevice, const VkShaderStageFlags pShaderStages,
        std::initializer_list<std::pair<uint32_t, VkDescriptorType>> bindings) {
    device = pDevice;
    DescriptorLayoutBuilder builder;
    for (const auto& [binding, type] : bindings) {
        builder.add_binding(binding, type);
    }
    set = builder.build(pDevice, pShaderStages);
}

DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(device, set, nullptr);
}
