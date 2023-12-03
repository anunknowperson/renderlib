#pragma once


#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include <memory>

#include <vector>


#include "internal/vulkan_instance.h"
#include "internal/vulkan_physical_device.h"
#include "internal/vulkan_surface.h"

class VulkanRender;

class VulkanGraphicsPipeline {
private:
    static std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);

    VulkanRender* render;


    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
public:

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VulkanGraphicsPipeline(VulkanRender* p_render);
    ~VulkanGraphicsPipeline();
};