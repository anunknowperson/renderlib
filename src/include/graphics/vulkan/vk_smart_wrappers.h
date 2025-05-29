#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>
#include <vk_mem_alloc.h>
#include "vk_types.h"

class VulkanEngine;

// RAII wrapper for VkFence
class VulkanFence {
public:
    VulkanFence(VkDevice device, VkFence fence) : device_(device), fence_(fence) {}
    
    ~VulkanFence() {
        if (fence_ != VK_NULL_HANDLE) {
            vkDestroyFence(device_, fence_, nullptr);
        }
    }

    // Move constructor and assignment
    VulkanFence(VulkanFence&& other) noexcept 
        : device_(other.device_), fence_(other.fence_) {
        other.fence_ = VK_NULL_HANDLE;
    }

    VulkanFence& operator=(VulkanFence&& other) noexcept {
        if (this != &other) {
            if (fence_ != VK_NULL_HANDLE) {
                vkDestroyFence(device_, fence_, nullptr);
            }
            device_ = other.device_;
            fence_ = other.fence_;
            other.fence_ = VK_NULL_HANDLE;
        }
        return *this;
    }

    // Delete copy constructor and assignment
    VulkanFence(const VulkanFence&) = delete;
    VulkanFence& operator=(const VulkanFence&) = delete;

    VkFence get() const { return fence_; }
    VkFence* getPtr() { return &fence_; }
    operator VkFence() const { return fence_; }

private:
    VkDevice device_;
    VkFence fence_ = VK_NULL_HANDLE;
};

// RAII wrapper for VkSemaphore
class VulkanSemaphore {
public:
    VulkanSemaphore(VkDevice device, VkSemaphore semaphore) : device_(device), semaphore_(semaphore) {}
    
    ~VulkanSemaphore() {
        if (semaphore_ != VK_NULL_HANDLE) {
            vkDestroySemaphore(device_, semaphore_, nullptr);
        }
    }

    // Move constructor and assignment
    VulkanSemaphore(VulkanSemaphore&& other) noexcept 
        : device_(other.device_), semaphore_(other.semaphore_) {
        other.semaphore_ = VK_NULL_HANDLE;
    }

    VulkanSemaphore& operator=(VulkanSemaphore&& other) noexcept {
        if (this != &other) {
            if (semaphore_ != VK_NULL_HANDLE) {
                vkDestroySemaphore(device_, semaphore_, nullptr);
            }
            device_ = other.device_;
            semaphore_ = other.semaphore_;
            other.semaphore_ = VK_NULL_HANDLE;
        }
        return *this;
    }

    // Delete copy constructor and assignment
    VulkanSemaphore(const VulkanSemaphore&) = delete;
    VulkanSemaphore& operator=(const VulkanSemaphore&) = delete;

    VkSemaphore get() const { return semaphore_; }
    VkSemaphore* getPtr() { return &semaphore_; }
    operator VkSemaphore() const { return semaphore_; }

private:
    VkDevice device_;
    VkSemaphore semaphore_ = VK_NULL_HANDLE;
};

// RAII wrapper for VkCommandPool
class VulkanCommandPool {
public:
    VulkanCommandPool(VkDevice device, VkCommandPool commandPool) : device_(device), commandPool_(commandPool) {}
    
    ~VulkanCommandPool() {
        if (commandPool_ != VK_NULL_HANDLE) {
            vkDestroyCommandPool(device_, commandPool_, nullptr);
        }
    }

    // Move constructor and assignment
    VulkanCommandPool(VulkanCommandPool&& other) noexcept 
        : device_(other.device_), commandPool_(other.commandPool_) {
        other.commandPool_ = VK_NULL_HANDLE;
    }

    VulkanCommandPool& operator=(VulkanCommandPool&& other) noexcept {
        if (this != &other) {
            if (commandPool_ != VK_NULL_HANDLE) {
                vkDestroyCommandPool(device_, commandPool_, nullptr);
            }
            device_ = other.device_;
            commandPool_ = other.commandPool_;
            other.commandPool_ = VK_NULL_HANDLE;
        }
        return *this;
    }

    // Delete copy constructor and assignment
    VulkanCommandPool(const VulkanCommandPool&) = delete;
    VulkanCommandPool& operator=(const VulkanCommandPool&) = delete;

    VkCommandPool get() const { return commandPool_; }
    VkCommandPool* getPtr() { return &commandPool_; }
    operator VkCommandPool() const { return commandPool_; }

private:
    VkDevice device_;
    VkCommandPool commandPool_ = VK_NULL_HANDLE;
};

// RAII wrapper for VkImageView
class VulkanImageView {
public:
    VulkanImageView(VkDevice device, VkImageView imageView) : device_(device), imageView_(imageView) {}
    
    ~VulkanImageView() {
        if (imageView_ != VK_NULL_HANDLE) {
            vkDestroyImageView(device_, imageView_, nullptr);
        }
    }

    // Move constructor and assignment
    VulkanImageView(VulkanImageView&& other) noexcept 
        : device_(other.device_), imageView_(other.imageView_) {
        other.imageView_ = VK_NULL_HANDLE;
    }

    VulkanImageView& operator=(VulkanImageView&& other) noexcept {
        if (this != &other) {
            if (imageView_ != VK_NULL_HANDLE) {
                vkDestroyImageView(device_, imageView_, nullptr);
            }
            device_ = other.device_;
            imageView_ = other.imageView_;
            other.imageView_ = VK_NULL_HANDLE;
        }
        return *this;
    }

    // Delete copy constructor and assignment
    VulkanImageView(const VulkanImageView&) = delete;
    VulkanImageView& operator=(const VulkanImageView&) = delete;

    VkImageView get() const { return imageView_; }
    VkImageView* getPtr() { return &imageView_; }
    operator VkImageView() const { return imageView_; }

private:
    VkDevice device_;
    VkImageView imageView_ = VK_NULL_HANDLE;
};

// RAII wrapper for AllocatedBuffer
class VulkanBuffer {
public:
    VulkanBuffer(VmaAllocator allocator, const AllocatedBuffer& buffer) 
        : allocator_(allocator), buffer_(buffer) {}
    
    ~VulkanBuffer() {
        if (buffer_.buffer != VK_NULL_HANDLE) {
            vmaDestroyBuffer(allocator_, buffer_.buffer, buffer_.allocation);
        }
    }

    // Move constructor and assignment
    VulkanBuffer(VulkanBuffer&& other) noexcept 
        : allocator_(other.allocator_), buffer_(other.buffer_) {
        other.buffer_.buffer = VK_NULL_HANDLE;
        other.buffer_.allocation = nullptr;
    }

    VulkanBuffer& operator=(VulkanBuffer&& other) noexcept {
        if (this != &other) {
            if (buffer_.buffer != VK_NULL_HANDLE) {
                vmaDestroyBuffer(allocator_, buffer_.buffer, buffer_.allocation);
            }
            allocator_ = other.allocator_;
            buffer_ = other.buffer_;
            other.buffer_.buffer = VK_NULL_HANDLE;
            other.buffer_.allocation = nullptr;
        }
        return *this;
    }

    // Delete copy constructor and assignment
    VulkanBuffer(const VulkanBuffer&) = delete;
    VulkanBuffer& operator=(const VulkanBuffer&) = delete;

    const AllocatedBuffer& get() const { return buffer_; }
    VkBuffer buffer() const { return buffer_.buffer; }
    VmaAllocation allocation() const { return buffer_.allocation; }

private:
    VmaAllocator allocator_;
    AllocatedBuffer buffer_{};
};

// RAII wrapper for AllocatedImage
class VulkanImage {
public:
    VulkanImage(VmaAllocator allocator, VkDevice device, const AllocatedImage& image) 
        : allocator_(allocator), device_(device), image_(image) {}
    
    ~VulkanImage() {
        if (image_.imageView != VK_NULL_HANDLE) {
            vkDestroyImageView(device_, image_.imageView, nullptr);
        }
        if (image_.image != VK_NULL_HANDLE) {
            vmaDestroyImage(allocator_, image_.image, image_.allocation);
        }
    }

    // Move constructor and assignment
    VulkanImage(VulkanImage&& other) noexcept 
        : allocator_(other.allocator_), device_(other.device_), image_(other.image_) {
        other.image_.image = VK_NULL_HANDLE;
        other.image_.imageView = VK_NULL_HANDLE;
        other.image_.allocation = nullptr;
    }

    VulkanImage& operator=(VulkanImage&& other) noexcept {
        if (this != &other) {
            if (image_.imageView != VK_NULL_HANDLE) {
                vkDestroyImageView(device_, image_.imageView, nullptr);
            }
            if (image_.image != VK_NULL_HANDLE) {
                vmaDestroyImage(allocator_, image_.image, image_.allocation);
            }
            allocator_ = other.allocator_;
            device_ = other.device_;
            image_ = other.image_;
            other.image_.image = VK_NULL_HANDLE;
            other.image_.imageView = VK_NULL_HANDLE;
            other.image_.allocation = nullptr;
        }
        return *this;
    }

    // Delete copy constructor and assignment
    VulkanImage(const VulkanImage&) = delete;
    VulkanImage& operator=(const VulkanImage&) = delete;

    const AllocatedImage& get() const { return image_; }
    VkImage image() const { return image_.image; }
    VkImageView imageView() const { return image_.imageView; }
    VmaAllocation allocation() const { return image_.allocation; }

private:
    VmaAllocator allocator_;
    VkDevice device_;
    AllocatedImage image_{};
};

// Factory functions for creating smart pointers
template<typename T, typename... Args>
std::unique_ptr<T> make_vulkan_unique(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}
