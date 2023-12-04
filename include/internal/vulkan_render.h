
#pragma once

#include <memory>

#include <vulkan/vulkan.hpp>


#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#include "GLFW/glfw3.h"


#include "internal/vulkan_instance.h"
#include "internal/vulkan_physical_device.h"
#include "internal/vulkan_device.h"
#include "internal/vulkan_surface.h"
#include "internal/vulkan_swap_chain.h"
#include "internal/vulkan_graphics_pipeline.h"
#include "internal/vulkan_render_pass.h"
#include "internal/vulkan_command_pool.h"

#include <array>

// TODO: Remove GLFW dependency from VulkanRender


class VulkanRender {
private:
    double lastTime;
    int nbFrames;


public:
    VkDescriptorSetLayout descriptorSetLayout;

    struct Vertex {
        glm::vec2 pos;
        glm::vec3 color;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            
            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            return attributeDescriptions;
        }
    };

    const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };

    const std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0
    };

    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };


    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    const int MAX_FRAMES_IN_FLIGHT = 2;

	GLFWwindow* window;

	std::shared_ptr<VulkanInstance> vulkan_instance;
    std::shared_ptr<VulkanDevice> vulkan_device;
    std::shared_ptr<VulkanSurface> vulkan_surface;
    std::shared_ptr<VulkanSwapchain> vulkan_swap_chain;
    std::shared_ptr<VulkanGraphicsPipeline> vulkan_graphics_pipeline;
    std::shared_ptr<VulkanRenderPass> vulkan_render_pass;
    std::shared_ptr<VulkanCommandPool> vulkan_command_pool;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

	VkPhysicalDevice vulkan_physical_device;

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	void init(GLFWwindow* p_window);

	void render();

    void createDescriptorSetLayout();

    void createSyncObjects();

    void createVertexBuffer();
    void createIndexBuffer();

    void createUniformBuffers();

    void createDescriptorPool();

    void createDescriptorSets();


    void updateUniformBuffer(uint32_t currentImage);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    void drawFrame();

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    bool framebufferResized = false;

	VulkanRender();
	~VulkanRender();

    uint32_t currentFrame = 0;
};