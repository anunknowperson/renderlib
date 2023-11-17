
#pragma once

#include <memory>

#include <vulkan/vulkan.hpp>

#include "GLFW/glfw3.h"

#include "internal/vulkan_instance.h"
#include "internal/vulkan_physical_device.h"
#include "internal/vulkan_device.h"

// TODO: Remove GLFW dependency from Renderer


class Renderer {
private:
	GLFWwindow* window;

	std::shared_ptr<VulkanInstance> vulkan_instance;
    std::shared_ptr<VulkanDevice> vulkan_device;

	VkPhysicalDevice vulkan_physical_device;



    VkSurfaceKHR surface;

public:
	void init(GLFWwindow* p_window);

	void render();


	Renderer();
	~Renderer();
};