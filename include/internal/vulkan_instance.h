
#pragma once

#include "GLFW/glfw3.h"

#include "vulkan/vulkan.h"
#include <vector>

class VulkanInstance {

private:
	

	VkDebugUtilsMessengerEXT debugMessenger;

	VkApplicationInfo getAppInfo();
	void populateInstanceCreateInfo(VkInstanceCreateInfo& createInfo, VkApplicationInfo& appInfo, std::vector<const char*>& extensions);

	void enumerateExtensions();

	void enumerateValidationLayers();
	bool checkValidationLayerSupport();

	std::vector<const char*> getRequiredExtensions();

	void createInstance();
	void setupDebugMessenger();

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

public:
	VkInstance instance;

	VulkanInstance();
	~VulkanInstance();
};