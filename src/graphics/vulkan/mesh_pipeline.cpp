#include "pipeline_interface.h"
#include "graphics/vulkan/vk_engine.h"
#include "vk_initializers.h"
#include "vk_pipelines.h"
#include <fmt/core.h>
#include <string>

void MeshPipeline::init(VulkanEngine* engine) {
    _device = engine->_device;

    VkShaderModule fragShader = load_shader("./shaders/tex_image.frag.spv", "fragment");
    VkShaderModule vertShader = load_shader("./shaders/colored_triangle_mesh.vert.spv", "vertex");

    create_pipeline_layout(engine);
    build_pipeline(fragShader, vertShader);

    vkDestroyShaderModule(_device, fragShader, nullptr);
    vkDestroyShaderModule(_device, vertShader, nullptr);
}

void MeshPipeline::destroy() {
    if (meshPipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(_device, meshPipeline, nullptr);
        meshPipeline = VK_NULL_HANDLE;
    }
    if (meshPipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(_device, meshPipelineLayout, nullptr);
        meshPipelineLayout = VK_NULL_HANDLE;
    }
}

VkShaderModule MeshPipeline::load_shader(const std::filesystem::path& path, const char* type) {
    std::string str = path.string();
    const char* cPath = str.c_str();
    VkShaderModule shaderModule;
    if (!vkutil::load_shader_module(cPath, _device, &shaderModule)) {
        fmt::println("Error when building the {} shader module", type);
    }
    else {
        fmt::println("{} shader successfully loaded", type);
    }
    return shaderModule;
}

void MeshPipeline::create_pipeline_layout(VulkanEngine* engine) {
    VkPushConstantRange bufferRange{};
    bufferRange.offset = 0;
    bufferRange.size = sizeof(GPUDrawPushConstants);
    bufferRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::pipeline_layout_create_info();
    pipeline_layout_info.pPushConstantRanges = &bufferRange;
    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pSetLayouts = &_singleImageDescriptorLayout;
    pipeline_layout_info.setLayoutCount = 1;

    VK_CHECK(vkCreatePipelineLayout(_device, &pipeline_layout_info, nullptr, &meshPipelineLayout));
}

void MeshPipeline::build_pipeline(VkShaderModule fragShader, VkShaderModule vertShader) {
    PipelineBuilder pipelineBuilder;
    pipelineBuilder._pipelineLayout = meshPipelineLayout;
    pipelineBuilder.set_shaders(vertShader, fragShader);
    pipelineBuilder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    pipelineBuilder.set_polygon_mode(VK_POLYGON_MODE_FILL);
    pipelineBuilder.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
    pipelineBuilder.set_multisampling_none();
    pipelineBuilder.disable_blending();
    pipelineBuilder.enable_depthtest(true, VK_COMPARE_OP_GREATER);
    pipelineBuilder.set_color_attachment_format(_drawImage.imageFormat);
    pipelineBuilder.set_depth_format(VK_FORMAT_UNDEFINED);

    meshPipeline = pipelineBuilder.build_pipeline(_device);
}
