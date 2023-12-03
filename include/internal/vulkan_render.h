
#pragma once

#include <memory>

#include <vulkan/vulkan.hpp>

#include "GLFW/glfw3.h"

#include "internal/vulkan_instance.h"
#include "internal/vulkan_physical_device.h"
#include "internal/vulkan_device.h"
#include "internal/vulkan_surface.h"
#include "internal/vulkan_swap_chain.h"
#include "internal/vulkan_graphics_pipeline.h"
#include "internal/vulkan_render_pass.h"
#include "internal/vulkan_command_pool.h"

// TODO: Remove GLFW dependency from VulkanRender


class VulkanRender {
private:



    uint32_t currentFrame = 0;

public:
    const int MAX_FRAMES_IN_FLIGHT = 2;

	GLFWwindow* window;

	std::shared_ptr<VulkanInstance> vulkan_instance;
    std::shared_ptr<VulkanDevice> vulkan_device;
    std::shared_ptr<VulkanSurface> vulkan_surface;
    std::shared_ptr<VulkanSwapchain> vulkan_swap_chain;
    std::shared_ptr<VulkanGraphicsPipeline> vulkan_graphics_pipeline;
    std::shared_ptr<VulkanRenderPass> vulkan_render_pass;
    std::shared_ptr<VulkanCommandPool> vulkan_command_pool;

	VkPhysicalDevice vulkan_physical_device;

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	void init(GLFWwindow* p_window);

	void render();

    void createSyncObjects();

    void drawFrame();

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    bool framebufferResized = false;

	VulkanRender();
	~VulkanRender();
};