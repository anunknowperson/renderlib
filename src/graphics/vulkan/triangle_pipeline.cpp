#include "pipeline_interface.h"
#include "graphics/vulkan/vk_engine.h"
#include "vk_initializers.h"
#include "vk_pipelines.h"
#include <fmt/core.h>
#include <string>

void TrianglePipeline::init(VulkanEngine* engine) {
    _device = engine->_device;

    VkShaderModule fragShader = load_shader("./shaders/colored_triangle.frag.spv", "fragment");
    VkShaderModule vertShader = load_shader("./shaders/colored_triangle.vert.spv", "vertex");

    create_pipeline_layout(engine);
    build_pipeline(fragShader, vertShader);

    vkDestroyShaderModule(_device, fragShader, nullptr);
    vkDestroyShaderModule(_device, vertShader, nullptr);
}

void TrianglePipeline::destroy() {
    if (trianglePipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(_device, trianglePipeline, nullptr);
        trianglePipeline = VK_NULL_HANDLE;
    }
    if (trianglePipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(_device, trianglePipelineLayout, nullptr);
        trianglePipelineLayout = VK_NULL_HANDLE;
    }
}

VkShaderModule TrianglePipeline::load_shader(const std::filesystem::path& path, const char* type) {
    std::string str = path.string();
    const char* cPath = str.c_str();  // Ensure lifetime of string is managed here.
    VkShaderModule shaderModule;
    if (!vkutil::load_shader_module(cPath, _device, &shaderModule)) {
        fmt::println("Error when building the {} shader module", type);
    }
    else {
        fmt::println("{} shader successfully loaded", type);
    }
    return shaderModule;
}

void TrianglePipeline::create_pipeline_layout(VulkanEngine* engine) {
    VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::pipeline_layout_create_info();
    VK_CHECK(vkCreatePipelineLayout(engine->_device, &pipeline_layout_info, nullptr, &trianglePipelineLayout));
}

void TrianglePipeline::build_pipeline(VkShaderModule fragShader, VkShaderModule vertShader) {
    PipelineBuilder pipelineBuilder;
    pipelineBuilder._pipelineLayout = trianglePipelineLayout;
    pipelineBuilder.set_shaders(vertShader, fragShader);
    pipelineBuilder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    pipelineBuilder.set_polygon_mode(VK_POLYGON_MODE_FILL);
    pipelineBuilder.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
    pipelineBuilder.set_multisampling_none();
    pipelineBuilder.disable_blending();
    pipelineBuilder.disable_depthtest();
    pipelineBuilder.set_color_attachment_format(_drawImage.imageFormat);
    pipelineBuilder.set_depth_format(VK_FORMAT_UNDEFINED);

    trianglePipeline = pipelineBuilder.build_pipeline(_device);
}
