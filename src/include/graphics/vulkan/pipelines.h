#pragma once

#include <filesystem>

#include "vk_descriptors.h"
#include "vk_images.h"
#include "vk_initializers.h"
#include "vk_pipelines.h"
#include "vk_types.h"
#include "IPipeline.h"
#include "GraphicsPipeline.h"
#include "ComputePipeline.h"

class VulkanEngine;

struct GLTFMetallic_Roughness {
    MaterialPipeline opaquePipeline;
    MaterialPipeline transparentPipeline;

    VkDescriptorSetLayout materialLayout;

    struct MaterialConstants {
        glm::vec4 colorFactors;
        glm::vec4 metal_rough_factors;
        glm::vec4 extra[14];
    };

    struct MaterialResources {
        AllocatedImage colorImage;
        VkSampler colorSampler;
        AllocatedImage metalRoughImage;
        VkSampler metalRoughSampler;
        VkBuffer dataBuffer;
        uint32_t dataBufferOffset;
    };

    DescriptorWriter writer;

    void build_pipelines(VulkanEngine* engine);
    void clear_resources(VkDevice device);
    MaterialInstance write_material(
            VkDevice device, MaterialPass pass,
            const MaterialResources& resources,
            DescriptorAllocatorGrowable& descriptorAllocator);

private:
    VkShaderModule load_shader(VulkanEngine* engine, const char* path,
                               const char* type);
    void create_material_layout(VulkanEngine* engine);
    VkPipelineLayout create_pipeline_layout(VulkanEngine* engine);
    void build_opaque_pipeline(VulkanEngine* engine,
                               VkShaderModule vertexShader,
                               VkShaderModule fragShader,
                               VkPipelineLayout layout);
    void build_transparent_pipeline(VulkanEngine* engine,
                                    VkShaderModule vertexShader,
                                    VkShaderModule fragShader,
                                    VkPipelineLayout layout);
};

class Pipelines {
public:
    std::unique_ptr<GraphicsPipeline> trianglePipeline;
    std::unique_ptr<GraphicsPipeline> meshPipeline;
    std::unique_ptr<ComputePipeline> gradientPipeline;

    void init(VkDevice device,
              VkDescriptorSetLayout singleImageDescriptorLayout,
              VkDescriptorSetLayout drawImageDescriptorLayout,
              AllocatedImage drawImage);
    void destroy();

private:
    VkDevice _device;
    VkDescriptorSetLayout _singleImageDescriptorLayout;
    VkDescriptorSetLayout _drawImageDescriptorLayout;
    AllocatedImage _drawImage;
};
