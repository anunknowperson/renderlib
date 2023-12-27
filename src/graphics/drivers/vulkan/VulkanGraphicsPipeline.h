#pragma once

#include <memory>
#include <vector>

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"

class VulkanRender;

class VulkanGraphicsPipeline {
public:
    VulkanGraphicsPipeline(VulkanRender* p_render);
    ~VulkanGraphicsPipeline();

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;


private:
    static std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);

    VulkanRender* render;

    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;

};