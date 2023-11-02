#include "internal/renderer.h"

void Renderer::init(GLFWwindow* p_window)
{
	window = p_window;

	vulkan_instance = std::make_shared<VulkanInstance>();
	
	vulkan_physical_device = VulkanPhysicalDevice::pick(vulkan_instance);


}

void Renderer::render()
{

}

Renderer::Renderer()
{

}

Renderer::~Renderer()
{
}
