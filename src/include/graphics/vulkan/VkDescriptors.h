#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>

struct DescriptorLayoutBuilder {
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    void addBinding(uint32_t binding, VkDescriptorType type);
    void clear();
    VkDescriptorSetLayout build(VkDevice device,
                                VkShaderStageFlags shader_stages,
                                const void* p_next = nullptr,
                                VkDescriptorSetLayoutCreateFlags flags = 0);
};

struct DescriptorAllocator {
    struct PoolSizeRatio {
        VkDescriptorType type;
        float ratio;
    };

    VkDescriptorPool pool;

    void initPool(VkDevice device, uint32_t max_sets,
                   std::span<PoolSizeRatio> pool_ratios);
    void clearDescriptors(VkDevice device) const;
    void destroyPool(VkDevice device) const;

    VkDescriptorSet allocate(VkDevice device,
                             VkDescriptorSetLayout layout) const;
};

struct DescriptorAllocatorGrowable {
public:
    struct PoolSizeRatio {
        VkDescriptorType type;
        float ratio;
    };

    void init(VkDevice device, uint32_t initial_sets,
              std::span<PoolSizeRatio> pool_ratios);
    void clearPools(VkDevice device);
    void destroyPools(VkDevice device);

    VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout,
                             const void* p_next = nullptr);

private:
    VkDescriptorPool getPool(VkDevice device);
    VkDescriptorPool createPool(VkDevice device, uint32_t set_count,
                                 std::span<PoolSizeRatio> pool_ratios);

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
