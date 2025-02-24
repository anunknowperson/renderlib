#pragma once

#include <filesystem>
#include "vk_types.h"
#include "vk_images.h"
#include "vk_descriptors.h"
#include "vk_pipelines.h"
#include "vk_initializers.h"
#include "vk_engine.h"

enum class MaterialPass { Opaque, Transparent };

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

struct MaterialInstance {
    MaterialPass passType;
    MaterialPipeline* pipeline;
    VkDescriptorSet materialSet;
};

//
// Unified interface for all pipelines.
//
class IPipeline {
public:
    virtual ~IPipeline() = default;
    virtual void init(VulkanEngine* engine) = 0;
    virtual void destroy() = 0;
    virtual VkPipeline getPipeline() const = 0;
    virtual VkPipelineLayout getPipelineLayout() const = 0;
};

//
// TrianglePipeline
//
class TrianglePipeline : public IPipeline {
public:
    TrianglePipeline(VkDescriptorSetLayout singleImageDescriptorLayout, AllocatedImage drawImage)
        : _singleImageDescriptorLayout(singleImageDescriptorLayout), _drawImage(drawImage) {}

    virtual ~TrianglePipeline() { destroy(); }

    void init(VulkanEngine* engine) override;
    void destroy() override;

    VkPipeline getPipeline() const override { return trianglePipeline; }
    VkPipelineLayout getPipelineLayout() const override { return trianglePipelineLayout; }

private:
    VkPipeline trianglePipeline = VK_NULL_HANDLE;
    VkPipelineLayout trianglePipelineLayout = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    VkDescriptorSetLayout _singleImageDescriptorLayout;
    AllocatedImage _drawImage;

    VkShaderModule load_shader(const std::filesystem::path& path, const char* type);
    void create_pipeline_layout(VulkanEngine* engine);
    void build_pipeline(VkShaderModule fragShader, VkShaderModule vertShader);
};

//
// MeshPipeline
//
class MeshPipeline : public IPipeline {
public:
    MeshPipeline(VkDescriptorSetLayout singleImageDescriptorLayout, AllocatedImage drawImage)
        : _singleImageDescriptorLayout(singleImageDescriptorLayout), _drawImage(drawImage) {}

    virtual ~MeshPipeline() { destroy(); }

    void init(VulkanEngine* engine) override;
    void destroy() override;

    VkPipeline getPipeline() const override { return meshPipeline; }
    VkPipelineLayout getPipelineLayout() const override { return meshPipelineLayout; }

private:
    VkPipeline meshPipeline = VK_NULL_HANDLE;
    VkPipelineLayout meshPipelineLayout = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    VkDescriptorSetLayout _singleImageDescriptorLayout;
    AllocatedImage _drawImage;

    VkShaderModule load_shader(const std::filesystem::path& path, const char* type);
    void create_pipeline_layout(VulkanEngine* engine);
    void build_pipeline(VkShaderModule fragShader, VkShaderModule vertShader);
};

//
// GLTFRoughnessPipeline (for GLTF metallic‑roughness materials)
//
class GLTFRoughnessPipeline : public IPipeline {
public:
    GLTFRoughnessPipeline() = default;
    virtual ~GLTFRoughnessPipeline() { destroy(); }

    void init(VulkanEngine* engine) override;
    void destroy() override;

    VkPipeline getPipeline() const override { return opaquePipeline.pipeline; }
    VkPipelineLayout getPipelineLayout() const override { return opaquePipeline.layout; }

    MaterialInstance write_material(VkDevice device,
        MaterialPass pass,
        const MaterialResources& resources,
        DescriptorAllocatorGrowable& descriptorAllocator);

private:
    MaterialPipeline opaquePipeline;
    MaterialPipeline transparentPipeline;

    VkDescriptorSetLayout materialLayout = VK_NULL_HANDLE;
    VkPipelineLayout materialPipelineLayout = VK_NULL_HANDLE;

    VkDevice _device = VK_NULL_HANDLE;

    DescriptorWriter writer;

    VkShaderModule load_shader(VulkanEngine* engine, const char* path, const char* type);
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
