#pragma once

#include "IPipeline.h"
#include "vk_types.h"
#include "vk_initializers.h"
#include "vk_pipelines.h"
#include <functional>

class ComputePipeline : public IPipeline {
public:
    struct ComputePipelineConfig {
        VkDescriptorSetLayout descriptorSetLayout;
        std::string shaderPath;
        std::function<void(VkDevice, VkPipeline, VkPipelineLayout)> customSetupCallback = nullptr;
    };

    ComputePipeline() = default;
    explicit ComputePipeline(const ComputePipelineConfig& config);
    
    void init(VkDevice device) override;
    void bind(VkCommandBuffer cmd) override;
    void destroy() override;
    
    VkPipeline getPipeline() const override { return _pipeline; }
    VkPipelineLayout getLayout() const override { return _pipelineLayout; }

    // Specific to compute pipelines
    void dispatch(VkCommandBuffer cmd, uint32_t x, uint32_t y, uint32_t z = 1);
    void bindDescriptorSets(VkCommandBuffer cmd, const VkDescriptorSet* descriptorSets, uint32_t setCount);

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkPipeline _pipeline = VK_NULL_HANDLE;
    VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
    ComputePipelineConfig _config;
};