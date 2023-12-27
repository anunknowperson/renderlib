//
// Created by Serg on 03.12.2023.
//

#include "VulkanSurface.h"
#include "VulkanRender.h"
#include "core/Logging.h"


VulkanSurface::VulkanSurface(VulkanRender* p_render) {
    render = p_render;


    VkResult err = glfwCreateWindowSurface(render->vulkan_instance->instance, render->window, NULL, &surface);
    if (err) {
        LOGE("Vulkan surface creation failed.");
    }
}


VulkanSurface::~VulkanSurface()
{
    vkDestroySurfaceKHR(render->vulkan_instance->instance, surface, nullptr);
}