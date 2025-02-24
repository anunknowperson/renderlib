#include "pipeline_interface.h"
#include "graphics/vulkan/vk_engine.h"
#include "vk_initializers.h"
#include "vk_pipelines.h"
#include "vk_descriptors.h"
#include <fmt/core.h>

void GLTFRoughnessPipeline::init(VulkanEngine* engine) {
    _device = engine->_device;

    VkShaderModule fragShader = load_shader(engine, "./shaders/mesh.frag.spv", "fragment");
    VkShaderModule vertShader = load_shader(engine, "./shaders/mesh.vert.spv", "vertex");

    create_material_layout(engine);
    VkPipelineLayout newLayout = create_pipeline_layout(engine);
    materialPipelineLayout = newLayout;

    // Both pipelines use the same layout.
    opaquePipeline.layout = newLayout;
    transparentPipeline.layout = newLayout;

    build_opaque_pipeline(engine, vertShader, fragShader, newLayout);
    build_transparent_pipeline(engine, vertShader, fragShader, newLayout);

    vkDestroyShaderModule(engine->_device, fragShader, nullptr);
    vkDestroyShaderModule(engine->_device, vertShader, nullptr);
}

void GLTFRoughnessPipeline::destroy() {
    if (opaquePipeline.pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(_device, opaquePipeline.pipeline, nullptr);
        opaquePipeline.pipeline = VK_NULL_HANDLE;
    }
    if (transparentPipeline.pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(_device, transparentPipeline.pipeline, nullptr);
        transparentPipeline.pipeline = VK_NULL_HANDLE;
    }
    if (materialPipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(_device, materialPipelineLayout, nullptr);
        materialPipelineLayout = VK_NULL_HANDLE;
    }
    if (materialLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(_device, materialLayout, nullptr);
        materialLayout = VK_NULL_HANDLE;
    }
}

VkShaderModule GLTFRoughnessPipeline::load_shader(VulkanEngine* engine, const char* path, const char* type) {
    VkShaderModule shaderModule;
    if (!vkutil::load_shader_module(path, engine->_device, &shaderModule)) {
        fmt::println("Error when building the {} shader module", type);
    }
    return shaderModule;
}

void GLTFRoughnessPipeline::create_material_layout(VulkanEngine* engine) {
    DescriptorLayoutBuilder layoutBuilder;
    layoutBuilder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    layoutBuilder.add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    layoutBuilder.add_binding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    materialLayout = layoutBuilder.build(
        engine->_device,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
}

VkPipelineLayout GLTFRoughnessPipeline::create_pipeline_layout(VulkanEngine* engine) {
    VkPushConstantRange matrixRange{};
    matrixRange.offset = 0;
    matrixRange.size = sizeof(GPUDrawPushConstants);
    matrixRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayout layouts[] = { engine->_gpuSceneDataDescriptorLayout, materialLayout };
    VkPipelineLayoutCreateInfo mesh_layout_info = vkinit::pipeline_layout_create_info();
    mesh_layout_info.setLayoutCount = 2;
    mesh_layout_info.pSetLayouts = layouts;
    mesh_layout_info.pPushConstantRanges = &matrixRange;
    mesh_layout_info.pushConstantRangeCount = 1;

    VkPipelineLayout newLayout;
    VK_CHECK(vkCreatePipelineLayout(engine->_device, &mesh_layout_info, nullptr, &newLayout));

    return newLayout;
}

void GLTFRoughnessPipeline::build_opaque_pipeline(VulkanEngine* engine,
    VkShaderModule vertexShader,
    VkShaderModule fragShader,
    VkPipelineLayout layout) {
    PipelineBuilder pipelineBuilder;
    pipelineBuilder._pipelineLayout = layout;
    pipelineBuilder.set_shaders(vertexShader, fragShader);
    pipelineBuilder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    pipelineBuilder.set_polygon_mode(VK_POLYGON_MODE_FILL);
    pipelineBuilder.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
    pipelineBuilder.set_multisampling_none();
    pipelineBuilder.disable_blending();
    pipelineBuilder.enable_depthtest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);
    pipelineBuilder.set_color_attachment_format(engine->_drawImage.imageFormat);
    pipelineBuilder.set_depth_format(engine->_depthImage.imageFormat);

    opaquePipeline.pipeline = pipelineBuilder.build_pipeline(engine->_device);
}

void GLTFRoughnessPipeline::build_transparent_pipeline(VulkanEngine* engine,
    VkShaderModule vertexShader,
    VkShaderModule fragShader,
    VkPipelineLayout layout) {
    PipelineBuilder pipelineBuilder;
    pipelineBuilder._pipelineLayout = layout;
    pipelineBuilder.set_shaders(vertexShader, fragShader);
    pipelineBuilder.enable_blending_additive();
    pipelineBuilder.enable_depthtest(false, VK_COMPARE_OP_GREATER_OR_EQUAL);

    transparentPipeline.pipeline = pipelineBuilder.build_pipeline(engine->_device);
}

MaterialInstance GLTFRoughnessPipeline::write_material(VkDevice device,
    MaterialPass pass,
    const MaterialResources& resources,
    DescriptorAllocatorGrowable& descriptorAllocator) {
    MaterialInstance matData{};
    matData.passType = pass;
    matData.pipeline = (pass == MaterialPass::Transparent) ? &transparentPipeline : &opaquePipeline;
    matData.materialSet = descriptorAllocator.allocate(device, materialLayout);

    writer.clear();
    writer.write_buffer(0, resources.dataBuffer, sizeof(MaterialConstants),
        resources.dataBufferOffset, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    writer.write_image(1, resources.colorImage.imageView,
        resources.colorSampler,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    writer.write_image(2, resources.metalRoughImage.imageView,
        resources.metalRoughSampler,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    writer.update_set(device, matData.materialSet);
    writer.update_set(device, matData.materialSet);

    return matData;
}
