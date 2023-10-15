#include "internal/renderer.h"

void Renderer::init(GLFWwindow* p_window)
{
	window = p_window;

	vulkan_instance = new VulkanInstance();
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
