#include "graphics/vulkan/vk_descriptors.h"

#include "graphics/vulkan/vk_types.h"

void DescriptorLayoutBuilder::addBinding(uint32_t binding,
                                          VkDescriptorType type) {
    VkDescriptorSetLayoutBinding new_bind{};
    new_bind.binding = binding;
    new_bind.descriptorCount = 1;
    new_bind.descriptorType = type;

    bindings.push_back(new_bind);
}

void DescriptorLayoutBuilder::clear() {
    bindings.clear();
}

VkDescriptorSetLayout DescriptorLayoutBuilder::build(
        VkDevice device, VkShaderStageFlags shader_stages, const void* p_next,
        VkDescriptorSetLayoutCreateFlags flags) {
    for (auto& b : bindings) {
        b.stageFlags |= shader_stages;
    }

    VkDescriptorSetLayoutCreateInfo info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    info.pNext = p_next;

    info.pBindings = bindings.data();
    info.bindingCount = (uint32_t)bindings.size();
    info.flags = flags;

    VkDescriptorSetLayout set;
    VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &set));

    return set;
}

void DescriptorAllocator::initPool(VkDevice device, uint32_t max_sets,
                                    std::span<PoolSizeRatio> pool_ratios) {
    std::vector<VkDescriptorPoolSize> poolSizes;
    for (auto [type, ratio] : pool_ratios) {
        poolSizes.emplace_back(VkDescriptorPoolSize{
                .type = type,
                .descriptorCount = static_cast<uint32_t>(ratio * max_sets)});
    }

    VkDescriptorPoolCreateInfo pool_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    pool_info.flags = 0;
    pool_info.maxSets = max_sets;
    pool_info.poolSizeCount = (uint32_t)poolSizes.size();
    pool_info.pPoolSizes = poolSizes.data();

    vkCreateDescriptorPool(device, &pool_info, nullptr, &pool);
}

void DescriptorAllocator::clearDescriptors(VkDevice device) const {
    vkResetDescriptorPool(device, pool, 0);
}

void DescriptorAllocator::destroyPool(VkDevice device) const {
    vkDestroyDescriptorPool(device, pool, nullptr);
}

VkDescriptorSet DescriptorAllocator::allocate(
        VkDevice device, VkDescriptorSetLayout layout) const {
    VkDescriptorSetAllocateInfo alloc_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    alloc_info.pNext = nullptr;
    alloc_info.descriptorPool = pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &layout;

    VkDescriptorSet ds;
    VK_CHECK(vkAllocateDescriptorSets(device, &alloc_info, &ds));

    return ds;
}

VkDescriptorPool DescriptorAllocatorGrowable::getPool(VkDevice device) {
    VkDescriptorPool new_pool;
    if (!_ready_pools.empty()) {
        new_pool = _ready_pools.back();
        _ready_pools.pop_back();
    } else {
        // need to create a new pool
        new_pool = createPool(device, _sets_per_pool, _ratios);

        _sets_per_pool = static_cast<uint32_t>(_sets_per_pool * 1.5);
        if (_sets_per_pool > 4092) {
            _sets_per_pool = 4092;
        }
    }

    return new_pool;
}

VkDescriptorPool DescriptorAllocatorGrowable::createPool(
        VkDevice device, uint32_t set_count,
        std::span<PoolSizeRatio> pool_ratios) {
    std::vector<VkDescriptorPoolSize> pool_sizes;
    for (PoolSizeRatio ratio : pool_ratios) {
        pool_sizes.emplace_back(VkDescriptorPoolSize{
                .type = ratio.type,
                .descriptorCount =
                        static_cast<uint32_t>(ratio.ratio * set_count)});
    }

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = 0;
    pool_info.maxSets = set_count;
    pool_info.poolSizeCount = (uint32_t)pool_sizes.size();
    pool_info.pPoolSizes = pool_sizes.data();

    VkDescriptorPool new_pool;
    vkCreateDescriptorPool(device, &pool_info, nullptr, &new_pool);
    return new_pool;
}

void DescriptorAllocatorGrowable::init(VkDevice device, uint32_t initial_sets,
                                       std::span<PoolSizeRatio> pool_ratios) {
    _ratios.clear();

    for (auto r : pool_ratios) {
        _ratios.push_back(r);
    }

    const VkDescriptorPool new_pool =
            createPool(device, initial_sets, pool_ratios);

    _sets_per_pool = static_cast<uint32_t>(initial_sets *
                                        1.5);  // grow it next allocation

    _ready_pools.push_back(new_pool);
}

void DescriptorAllocatorGrowable::clearPools(VkDevice device) {
    for (const auto p : _ready_pools) {
        vkResetDescriptorPool(device, p, 0);
    }
    for (auto p : _full_pools) {
        vkResetDescriptorPool(device, p, 0);
        _ready_pools.push_back(p);
    }
    _full_pools.clear();
}

void DescriptorAllocatorGrowable::destroyPools(VkDevice device) {
    for (const auto p : _ready_pools) {
        vkDestroyDescriptorPool(device, p, nullptr);
    }
    _ready_pools.clear();
    for (const auto p : _full_pools) {
        vkDestroyDescriptorPool(device, p, nullptr);
    }
    _full_pools.clear();
}

VkDescriptorSet DescriptorAllocatorGrowable::allocate(
        VkDevice device, VkDescriptorSetLayout layout, const void* p_next) {
    // get or create a pool to allocate from
    VkDescriptorPool pool_to_use = getPool(device);

    VkDescriptorSetAllocateInfo alloc_info = {};
    alloc_info.pNext = p_next;
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pool_to_use;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &layout;

    VkDescriptorSet ds;
    VkResult result = vkAllocateDescriptorSets(device, &alloc_info, &ds);

    // allocation failed. Try again
    if (result == VK_ERROR_OUT_OF_POOL_MEMORY ||
        result == VK_ERROR_FRAGMENTED_POOL) {
        _full_pools.push_back(pool_to_use);

        pool_to_use = getPool(device);
        alloc_info.descriptorPool = pool_to_use;

        VK_CHECK(vkAllocateDescriptorSets(device, &alloc_info, &ds));
    }

    _ready_pools.push_back(pool_to_use);
    return ds;
}

void DescriptorWriter::writeBuffer(int binding, VkBuffer buffer, size_t size,
                                    size_t offset, VkDescriptorType type) {
    const VkDescriptorBufferInfo& info =
            buffer_infos.emplace_back(VkDescriptorBufferInfo{
                    .buffer = buffer, .offset = offset, .range = size});

    VkWriteDescriptorSet write = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};

    write.dstBinding = static_cast<uint32_t>(binding);
    write.dstSet =
            VK_NULL_HANDLE;  // left empty for now until we need to write it
    write.descriptorCount = 1;
    write.descriptorType = type;
    write.pBufferInfo = &info;

    writes.push_back(write);
}

void DescriptorWriter::writeImage(int binding, VkImageView image,
                                   VkSampler sampler, VkImageLayout layout,
                                   VkDescriptorType type) {
    const VkDescriptorImageInfo& info = image_infos.emplace_back(
            VkDescriptorImageInfo{.sampler = sampler,
                                  .imageView = image,
                                  .imageLayout = layout});

    VkWriteDescriptorSet write = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};

    write.dstBinding = static_cast<uint32_t>(binding);
    write.dstSet =
            VK_NULL_HANDLE;  // left empty for now until we need to write it
    write.descriptorCount = 1;
    write.descriptorType = type;
    write.pImageInfo = &info;

    writes.push_back(write);
}

void DescriptorWriter::clear() {
    image_infos.clear();
    writes.clear();
    buffer_infos.clear();
}

void DescriptorWriter::updateSet(VkDevice device, VkDescriptorSet set) {
    for (VkWriteDescriptorSet& write : writes) {
        write.dstSet = set;
    }

    vkUpdateDescriptorSets(device, (uint32_t)writes.size(), writes.data(), 0,
                           nullptr);
}
