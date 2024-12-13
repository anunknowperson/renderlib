#pragma once

#include <span>

#include "graphics/vulkan/VkTypes.h"

struct DescriptorLayoutBuilder {
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    void addBinding(uint32_t binding, VkDescriptorType type);
    void clear();
    VkDescriptorSetLayout build(VkDevice device,
                                VkShaderStageFlags shaderStages,
                                void* pNext = nullptr,
                                VkDescriptorSetLayoutCreateFlags flags = 0);
};

struct DescriptorAllocator {
    struct PoolSizeRatio {
        VkDescriptorType type;
        float ratio;
    };

    VkDescriptorPool pool;

    void initPool(VkDevice device, uint32_t maxSets,
                   std::span<PoolSizeRatio> poolRatios);
    void clearDescriptors(VkDevice device);
    void destroyPool(VkDevice device);

    VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);
};

struct DescriptorAllocatorGrowable {
public:
    struct PoolSizeRatio {
        VkDescriptorType type;
        float ratio;
    };

    void init(VkDevice device, uint32_t initialSets,
              std::span<PoolSizeRatio> poolRatios);
    void clearPools(VkDevice device);
    void destroyPools(VkDevice device);

    VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout,
                             void* pNext = nullptr);

private:
    VkDescriptorPool getPool(VkDevice device);
    VkDescriptorPool createPool(VkDevice device, uint32_t setCount,
                                 std::span<PoolSizeRatio> poolRatios);

    std::vector<PoolSizeRatio> _ratios;
    std::vector<VkDescriptorPool> _full_pools;
    std::vector<VkDescriptorPool> _ready_pools;
    uint32_t _sets_per_pool;
};

struct DescriptorWriter {
    std::deque<VkDescriptorImageInfo> image_infos;
    std::deque<VkDescriptorBufferInfo> buffer_infos;
    std::vector<VkWriteDescriptorSet> writes;

    void writeImage(int binding, VkImageView image, VkSampler sampler,
                     VkImageLayout layout, VkDescriptorType type);
    void writeBuffer(int binding, VkBuffer buffer, size_t size, size_t offset,
                      VkDescriptorType type);

    void clear();
    void updateSet(VkDevice device, VkDescriptorSet set);
};
