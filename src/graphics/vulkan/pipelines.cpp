#include "graphics/vulkan/pipelines.h"
#include "graphics/vulkan/vk_engine.h"

void GLTFMetallic_Roughness::build_pipelines(VulkanEngine* engine) {
    VkShaderModule meshFragShader =
            load_shader(engine, "./shaders/mesh.frag.spv", "fragment");
    VkShaderModule meshVertexShader =
            load_shader(engine, "./shaders/mesh.vert.spv", "vertex");

    create_material_layout(engine);
    VkPipelineLayout newLayout = create_pipeline_layout(engine);

    opaquePipeline.layout = newLayout;
    transparentPipeline.layout = newLayout;

    build_opaque_pipeline(engine, meshVertexShader, meshFragShader, newLayout);
    build_transparent_pipeline(engine, meshVertexShader, meshFragShader,
                               newLayout);

    vkDestroyShaderModule(engine->_device, meshFragShader, nullptr);
    vkDestroyShaderModule(engine->_device, meshVertexShader, nullptr);
}

VkShaderModule GLTFMetallic_Roughness::load_shader(VulkanEngine* engine,
                                                   const char* path,
                                                   const char* type) {
    VkShaderModule shaderModule;
    if (!vkutil::load_shader_module(path, engine->_device, &shaderModule)) {
        fmt::println("Error when building the {} shader module", type);
    }
    return shaderModule;
}

void GLTFMetallic_Roughness::create_material_layout(VulkanEngine* engine) {
    DescriptorLayoutBuilder layoutBuilder;
    layoutBuilder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    layoutBuilder.add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    layoutBuilder.add_binding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    materialLayout = layoutBuilder.build(
            engine->_device,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
}

VkPipelineLayout GLTFMetallic_Roughness::create_pipeline_layout(
        VulkanEngine* engine) {
    VkPushConstantRange matrixRange{};
    matrixRange.offset = 0;
    matrixRange.size = sizeof(GPUDrawPushConstants);
    matrixRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayout layouts[] = {engine->_gpuSceneDataDescriptorLayout,
                                       materialLayout};
    VkPipelineLayoutCreateInfo mesh_layout_info =
            vkinit::pipeline_layout_create_info();
    mesh_layout_info.setLayoutCount = 2;
    mesh_layout_info.pSetLayouts = layouts;
    mesh_layout_info.pPushConstantRanges = &matrixRange;
    mesh_layout_info.pushConstantRangeCount = 1;

    VkPipelineLayout newLayout;
    VK_CHECK(vkCreatePipelineLayout(engine->_device, &mesh_layout_info, nullptr,
                                    &newLayout));

    return newLayout;
}

void GLTFMetallic_Roughness::build_opaque_pipeline(VulkanEngine* engine,
                                                   VkShaderModule vertexShader,
                                                   VkShaderModule fragShader,
                                                   VkPipelineLayout layout) {
    PipelineBuilder pipelineBuilder;
    pipelineBuilder.set_shaders(vertexShader, fragShader);
    pipelineBuilder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    pipelineBuilder.set_polygon_mode(VK_POLYGON_MODE_FILL);
    pipelineBuilder.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
    pipelineBuilder.set_multisampling_none();
    pipelineBuilder.disable_blending();
    pipelineBuilder.enable_depthtest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);
    pipelineBuilder.set_color_attachment_format(engine->_drawImage.imageFormat);
    pipelineBuilder.set_depth_format(engine->_depthImage.imageFormat);
    pipelineBuilder._pipelineLayout = layout;

    opaquePipeline.pipeline = pipelineBuilder.build_pipeline(engine->_device);
}

void GLTFMetallic_Roughness::build_transparent_pipeline(
        VulkanEngine* engine, VkShaderModule vertexShader,
        VkShaderModule fragShader, VkPipelineLayout layout) {
    PipelineBuilder pipelineBuilder;
    pipelineBuilder.set_shaders(vertexShader, fragShader);
    pipelineBuilder.enable_blending_additive();
    pipelineBuilder.enable_depthtest(false, VK_COMPARE_OP_GREATER_OR_EQUAL);
    pipelineBuilder._pipelineLayout = layout;

    transparentPipeline.pipeline =
            pipelineBuilder.build_pipeline(engine->_device);
}

MaterialInstance GLTFMetallic_Roughness::write_material(
        VkDevice device, MaterialPass pass, const MaterialResources& resources,
        DescriptorAllocatorGrowable& descriptorAllocator) {
    MaterialInstance matData{};
    matData.passType = pass;
    matData.pipeline = (pass == MaterialPass::Transparent)
                               ? &transparentPipeline
                               : &opaquePipeline;
    matData.materialSet = descriptorAllocator.allocate(device, materialLayout);

    writer.clear();
    writer.write_buffer(0, resources.dataBuffer, sizeof(MaterialConstants),
                        resources.dataBufferOffset,
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
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

void Pipelines::init(VkDevice device,
                     VkDescriptorSetLayout singleImageDescriptorLayout,
                     AllocatedImage drawImage) {
    _device = device;
    _singleImageDescriptorLayout = singleImageDescriptorLayout;
    _drawImage = drawImage;

    GraphicsPipeline::GraphicsPipelineConfig triangleConfig;
    triangleConfig.vertexShaderPath = "./shaders/colored_triangle.vert.spv";
    triangleConfig.fragmentShaderPath = "./shaders/colored_triangle.frag.spv";
    triangleConfig.colorFormat = _drawImage.imageFormat;
    triangleConfig.depthFormat = VK_FORMAT_UNDEFINED;
    triangleConfig.depthTest = false;
    triangleConfig.cullMode = VK_CULL_MODE_NONE;
    triangleConfig.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    trianglePipeline = std::make_unique<GraphicsPipeline>(triangleConfig);
    trianglePipeline->init(device);
    
    GraphicsPipeline::GraphicsPipelineConfig meshConfig;
    meshConfig.vertexShaderPath = "./shaders/colored_triangle_mesh.vert.spv";
    meshConfig.fragmentShaderPath = "./shaders/tex_image.frag.spv";
    meshConfig.colorFormat = _drawImage.imageFormat;
    meshConfig.depthFormat = VK_FORMAT_UNDEFINED;
    meshConfig.depthTest = true;
    meshConfig.depthCompareOp = VK_COMPARE_OP_GREATER;
    meshConfig.cullMode = VK_CULL_MODE_NONE;
    meshConfig.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    
    VkPushConstantRange bufferRange{};
    bufferRange.offset = 0;
    bufferRange.size = sizeof(GPUDrawPushConstants);
    bufferRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    meshConfig.pushConstants.push_back(bufferRange);
    
    meshConfig.descriptorSetLayouts.push_back(_singleImageDescriptorLayout);

    meshPipeline = std::make_unique<GraphicsPipeline>(meshConfig);
    meshPipeline->init(device);

    fmt::println("Pipelines initialized successfully");
}

void Pipelines::destroy() {
    if (trianglePipeline) {
        trianglePipeline->destroy();
    }
    
    if (meshPipeline) {
        meshPipeline->destroy();
    }
}
