#include "graphics/vulkan/ComputePipeline.h"
#include "graphics/vulkan/vk_initializers.h"

ComputePipeline::ComputePipeline(const ComputePipelineConfig& config) 
    : _config(config) {
}

void ComputePipeline::init(VkDevice device) {
    _device = device;
    
    VkPipelineLayoutCreateInfo computeLayout{};
    computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    computeLayout.pNext = nullptr;
    computeLayout.pSetLayouts = &_config.descriptorSetLayout;
    computeLayout.setLayoutCount = 1;

    VK_CHECK(vkCreatePipelineLayout(_device, &computeLayout, nullptr, &_pipelineLayout));
    
    VkShaderModule computeShader;
    if (!vkutil::load_shader_module(_config.shaderPath.c_str(), _device, &computeShader)) {
        fmt::println("Error when building the compute shader \n");
        return;
    }
    
    VkPipelineShaderStageCreateInfo stageInfo{};
    stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageInfo.pNext = nullptr;
    stageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    stageInfo.module = computeShader;
    stageInfo.pName = "main";
    
    VkComputePipelineCreateInfo computePipelineCreateInfo{};
    computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineCreateInfo.pNext = nullptr;
    computePipelineCreateInfo.layout = _pipelineLayout;
    computePipelineCreateInfo.stage = stageInfo;
    
    VK_CHECK(vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1,
                                    &computePipelineCreateInfo, nullptr,
                                    &_pipeline));
    
    if (_config.customSetupCallback) {
        _config.customSetupCallback(_device, _pipeline, _pipelineLayout);
    }
    
    vkDestroyShaderModule(_device, computeShader, nullptr);
}

void ComputePipeline::bind(VkCommandBuffer cmd) {
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _pipeline);
}

void ComputePipeline::bindDescriptorSets(VkCommandBuffer cmd, VkDescriptorSet* descriptorSets, uint32_t setCount) {
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _pipelineLayout, 
                           0, setCount, descriptorSets, 0, nullptr);
}

void ComputePipeline::dispatch(VkCommandBuffer cmd, uint32_t x, uint32_t y, uint32_t z) {
    vkCmdDispatch(cmd, x, y, z);
}

void ComputePipeline::destroy() {
    if (_device != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
        vkDestroyPipeline(_device, _pipeline, nullptr);
        _pipeline = VK_NULL_HANDLE;
        _pipelineLayout = VK_NULL_HANDLE;
    }
}