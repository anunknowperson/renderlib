#include "graphics/vulkan/GraphicsPipeline.h"

GraphicsPipeline::GraphicsPipeline(const GraphicsPipelineConfig& config) 
    : _config(config) {
}

void GraphicsPipeline::init(VkDevice device) {
    _device = device;
    
    VkPipelineLayoutCreateInfo layoutInfo = vkinit::pipeline_layout_create_info();
    
    if (!_config.descriptorSetLayouts.empty()) {
        layoutInfo.setLayoutCount = static_cast<uint32_t>(_config.descriptorSetLayouts.size());
        layoutInfo.pSetLayouts = _config.descriptorSetLayouts.data();
    }
    
    if (!_config.pushConstants.empty()) {
        layoutInfo.pushConstantRangeCount = static_cast<uint32_t>(_config.pushConstants.size());
        layoutInfo.pPushConstantRanges = _config.pushConstants.data();
    }
    
    VK_CHECK(vkCreatePipelineLayout(device, &layoutInfo, nullptr, &_pipelineLayout));
    
    VkShaderModule vertexShader;
    if (!vkutil::load_shader_module(_config.vertexShaderPath.c_str(), _device, &vertexShader)) {
        fmt::println("Error when building the vertex shader");
        return;
    }
    
    VkShaderModule fragmentShader;
    if (!vkutil::load_shader_module(_config.fragmentShaderPath.c_str(), _device, &fragmentShader)) {
        fmt::println("Error when building the fragment shader");
        vkDestroyShaderModule(_device, vertexShader, nullptr);
        return;
    }
    
    PipelineBuilder pipelineBuilder;
    pipelineBuilder.set_shaders(vertexShader, fragmentShader);
    pipelineBuilder.set_input_topology(_config.topology);
    pipelineBuilder.set_polygon_mode(VK_POLYGON_MODE_FILL);
    pipelineBuilder.set_cull_mode(_config.cullMode,
                                  VK_FRONT_FACE_COUNTER_CLOCKWISE);
    pipelineBuilder.set_multisampling_none();
    
    if (_config.blending) {
        pipelineBuilder.enable_blending_additive();
    } else {
        pipelineBuilder.disable_blending();
    }
    
    if (_config.depthTest) {
        pipelineBuilder.enable_depthtest(true, _config.depthCompareOp);
    } else {
        pipelineBuilder.disable_depthtest();
    }
    
    pipelineBuilder.set_color_attachment_format(_config.colorFormat);
    pipelineBuilder.set_depth_format(_config.depthFormat);
    pipelineBuilder._pipelineLayout = _pipelineLayout;
    
    if (_config.customPipelineSetup) {
        _config.customPipelineSetup(pipelineBuilder);
    }
    
    _pipeline = pipelineBuilder.build_pipeline(_device);
    
    vkDestroyShaderModule(_device, vertexShader, nullptr);
    vkDestroyShaderModule(_device, fragmentShader, nullptr);
}

void GraphicsPipeline::bind(VkCommandBuffer cmd) {
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
}

void GraphicsPipeline::bindDescriptorSets(VkCommandBuffer cmd,
                                          const VkDescriptorSet* descriptorSets,
                                          uint32_t setCount,
                                          uint32_t firstSet) {
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 
                           firstSet, setCount, descriptorSets, 0, nullptr);
}

void GraphicsPipeline::pushConstants(VkCommandBuffer cmd, VkShaderStageFlags stageFlags, 
                                   uint32_t offset, uint32_t size, const void* data) {
    vkCmdPushConstants(cmd, _pipelineLayout, stageFlags, offset, size, data);
}

void GraphicsPipeline::destroy() {
    if (_device != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
        vkDestroyPipeline(_device, _pipeline, nullptr);
        _pipeline = VK_NULL_HANDLE;
        _pipelineLayout = VK_NULL_HANDLE;
    }
}