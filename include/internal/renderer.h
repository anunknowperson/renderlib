
#pragma once

#include "GLFW/glfw3.h"

#include "internal/vulkan_instance.h"

// TODO: Remove GLFW dependency from Renderer


class Renderer {
private:
	GLFWwindow* window;

	VulkanInstance* vulkan_instance;



public:
	void init(GLFWwindow* p_window);

	void render();

	Renderer();
	~Renderer();
};