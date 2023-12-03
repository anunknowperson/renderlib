//
// Created by Serg on 03.12.2023.
//

#include "internal/vulkan_surface.h"
#include "internal/vulkan_render.h"
#include "core/logging.h"


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