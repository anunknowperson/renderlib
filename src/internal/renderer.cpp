#include "internal/renderer.h"

#include "core/logging.h"


void Renderer::init(GLFWwindow* p_window)
{
	window = p_window;

	vulkan_instance = std::make_shared<VulkanInstance>();

    VkResult err = glfwCreateWindowSurface(vulkan_instance->instance, window, NULL, &surface);
    if (err)
    {
        LOGE("Vulkan surface creation failed.");
    }

	vulkan_physical_device = VulkanPhysicalDevice::pick(vulkan_instance, surface);

    vulkan_device = std::make_shared<VulkanDevice>(vulkan_instance, vulkan_physical_device, surface);


}

void Renderer::render()
{

}

Renderer::Renderer()
{

}

Renderer::~Renderer()
{
    vkDestroySurfaceKHR(vulkan_instance->instance, surface, nullptr);
}
