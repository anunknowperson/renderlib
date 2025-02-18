#include <gtest/gtest.h>

#include "graphics/vulkan/vk_descriptors.h"

// Test class for DescriptorLayoutBuilder
class DescriptorLayoutBuilderTest : public ::testing::Test {
protected:
    DescriptorLayoutBuilder builder;
};

// Check adding a binding
TEST_F(DescriptorLayoutBuilderTest, AddBinding) {
    builder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    ASSERT_EQ(builder.bindings.size(), 1);
    EXPECT_EQ(builder.bindings[0].binding, 0);
    EXPECT_EQ(builder.bindings[0].descriptorType,
              VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
}

// Check clearing bindings
TEST_F(DescriptorLayoutBuilderTest, ClearBindings) {
    builder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    builder.clear();
    ASSERT_TRUE(builder.bindings.empty());
}

// Test class for DescriptorAllocator
class DescriptorAllocatorTest : public ::testing::Test {
protected:
    DescriptorAllocator allocator;
    VkDevice device = VK_NULL_HANDLE;
};

// Check descriptor pool initialization
TEST_F(DescriptorAllocatorTest, InitPool) {
    DescriptorAllocator::PoolSizeRatio ratios[] = {
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1.0f},
            {VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f}};
    allocator.init_pool(device, 10, ratios);
    ASSERT_NE(allocator.pool, VK_NULL_HANDLE);
}

// Check descriptor set allocation
TEST_F(DescriptorAllocatorTest, AllocateDescriptorSet) {
    DescriptorAllocator::PoolSizeRatio ratios[] = {
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1.0f}};
    allocator.init_pool(device, 10, ratios);
    VkDescriptorSetLayout layout = VK_NULL_HANDLE;
    VkDescriptorSet set = allocator.allocate(device, layout);
    ASSERT_NE(set, VK_NULL_HANDLE);
}

// Test class for DescriptorAllocatorGrowable
class DescriptorAllocatorGrowableTest : public ::testing::Test {
protected:
    DescriptorAllocatorGrowable allocator;
    VkDevice device = VK_NULL_HANDLE;
};

// Check initialization of growable descriptor pool
TEST_F(DescriptorAllocatorGrowableTest, InitGrowablePool) {
    DescriptorAllocatorGrowable::PoolSizeRatio ratios[] = {
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1.0f}};
    allocator.init(device, 10, ratios);
    ASSERT_FALSE(allocator.readyPools.empty());
}

// Check descriptor set allocation with growable pool
TEST_F(DescriptorAllocatorGrowableTest, AllocateDescriptorSet) {
    DescriptorAllocatorGrowable::PoolSizeRatio ratios[] = {
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1.0f}};
    allocator.init(device, 10, ratios);
    VkDescriptorSetLayout layout = VK_NULL_HANDLE;
    VkDescriptorSet set = allocator.allocate(device, layout);
    ASSERT_NE(set, VK_NULL_HANDLE);
}

// Test class for DescriptorWriter
class DescriptorWriterTest : public ::testing::Test {
protected:
    DescriptorWriter writer;
};

// Check writing a buffer to a descriptor
TEST_F(DescriptorWriterTest, WriteBuffer) {
    VkBuffer buffer = VK_NULL_HANDLE;
    writer.write_buffer(0, buffer, 128, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    ASSERT_EQ(writer.writes.size(), 1);
    EXPECT_EQ(writer.writes[0].dstBinding, 0);
}

// Check writing an image to a descriptor
TEST_F(DescriptorWriterTest, WriteImage) {
    VkImageView image = VK_NULL_HANDLE;
    VkSampler sampler = VK_NULL_HANDLE;
    writer.write_image(1, image, sampler,
                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    ASSERT_EQ(writer.writes.size(), 1);
    EXPECT_EQ(writer.writes[0].dstBinding, 1);
}

// Check clearing descriptor data
TEST_F(DescriptorWriterTest, Clear) {
    VkBuffer buffer = VK_NULL_HANDLE;
    writer.write_buffer(0, buffer, 128, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    writer.clear();
    ASSERT_TRUE(writer.writes.empty());
}
