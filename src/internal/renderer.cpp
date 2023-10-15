#include "internal/renderer.h"

void Renderer::init(GLFWwindow* window)
{
	vulkan_instance = new VulkanInstance(window);
}

void Renderer::render()
{

}

Renderer::Renderer()
{

}

Renderer::~Renderer()
{
	delete vulkan_instance;
}
