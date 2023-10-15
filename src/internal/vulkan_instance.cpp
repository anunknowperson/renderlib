#include "core/logging.h"

#include "internal/vulkan_instance.h"

VulkanInstance::VulkanInstance(GLFWwindow* p_window)
{
	window = p_window;

    createInstance();
}

VkApplicationInfo VulkanInstance::getAppInfo()
{
    VkApplicationInfo appInfo{};

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "renderlib";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.pEngineName = "renderlib";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    return appInfo;
}

VkInstanceCreateInfo VulkanInstance::getInstanceCreateInfo(VkApplicationInfo& appInfo)
{
    

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    createInfo.enabledLayerCount = 0;

    return createInfo;
}

void VulkanInstance::enumerateExtensions()
{
    uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> extensions(extensionCount);

	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	LOGI("Available extensions:");

    for (const auto& extension : extensions) {
		LOGI(extension.extensionName);
	}
}

void VulkanInstance::createInstance() {
    enumerateExtensions();

    VkApplicationInfo appInfo = getAppInfo();

    VkInstanceCreateInfo createInfo = getInstanceCreateInfo(appInfo);

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        LOGE("failed to create instance!");
    }
}

VulkanInstance::~VulkanInstance()
{
    vkDestroyInstance(instance, nullptr);
}
