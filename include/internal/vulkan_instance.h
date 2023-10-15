
#pragma once

#include "GLFW/glfw3.h"

#include "vulkan/vulkan.h"

class VulkanInstance {

private:
	VkInstance instance;
		
	VkApplicationInfo getAppInfo();
	VkInstanceCreateInfo getInstanceCreateInfo(VkApplicationInfo& appInfo);

	void enumerateExtensions();

	void createInstance();

public:
	VulkanInstance();
	~VulkanInstance();
};