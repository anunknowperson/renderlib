#include "graphics/vulkan/VkPipelines.h"

#include <fstream>

#include "graphics/vulkan/VkInitializers.h"

bool vkutil::loadShaderModule(const char* file_path, VkDevice device,
                                VkShaderModule* out_shader_module) {
    // open the file. With cursor at the end
    std::ifstream file(file_path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    // find what the size of the file is by looking up the location of the
    // cursor because the cursor is at the end, it gives the size directly in
    // bytes
    const auto fileSize = file.tellg();

    if (fileSize == -1) {
        spdlog::error("Failed to open file {}", file_path);
        return false;
    }
    // spirv expects the buffer to be on uint32, so make sure to reserve a int
    // vector big enough for the entire file
    std::vector<uint32_t> buffer(static_cast<uint32_t>(fileSize) / sizeof(uint32_t));

    // put file cursor at beginning
    file.seekg(0);

    // load the entire file into the buffer
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

    // now that the file is loaded into the buffer, we can close it
    file.close();

    // create a new shader module, using the buffer we loaded
    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.pNext = nullptr;

    // codeSize has to be in bytes, so multply the ints in the buffer by size of
    // int to know the real size of the buffer
    create_info.codeSize = buffer.size() * sizeof(uint32_t);
    create_info.pCode = buffer.data();

    // check that the creation goes well.
    VkShaderModule shader_module;
    if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) !=
        VK_SUCCESS) {
        return false;
    }
    *out_shader_module = shader_module;
    return true;
}

void PipelineBuilder::clear() {
    // clear all of the structs we need back to 0 with their correct stype

    input_assembly = {
            .sType =
                    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};

    rasterizer = {
            .sType =
                    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};

    color_blend_attachment = {};

    multisampling = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};

    pipeline_layout = {};

    depth_stencil = {
            .sType =
                    VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};

    render_info = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};

    shader_stages.clear();
}

VkPipeline PipelineBuilder::buildPipeline(VkDevice device) {
    // make viewport state from our stored viewport and scissor.
    // at the moment we wont support multiple viewports or scissors
    VkPipelineViewportStateCreateInfo viewport_state = {};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.pNext = nullptr;

    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    // setup dummy color blending. We arent using transparent objects yet
    // the blending is just "no blend", but we do write to the color attachment
    VkPipelineColorBlendStateCreateInfo color_blending = {};
    color_blending.sType =
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.pNext = nullptr;

    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;

    // completely clear VertexInputStateCreateInfo, as we have no need for it
    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    // build the actual pipeline
    // we now use all of the info structs we have been writing into into this
    // one to create the pipeline
    VkGraphicsPipelineCreateInfo pipeline_info = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    // connect the renderInfo to the pNext extension mechanism
    pipeline_info.pNext = &render_info;

    pipeline_info.stageCount = (uint32_t)shader_stages.size();
    pipeline_info.pStages = shader_stages.data();
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.pDepthStencilState = &depth_stencil;
    pipeline_info.layout = pipeline_layout;

    VkDynamicState state[] = {VK_DYNAMIC_STATE_VIEWPORT,
                              VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamic_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dynamic_info.pDynamicStates = &state[0];
    dynamic_info.dynamicStateCount = 2;

    pipeline_info.pDynamicState = &dynamic_info;

    // its easy to error out on create graphics pipeline, so we handle it a bit
    // better than the common vk_check case
    VkPipeline new_pipeline;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info,
                                  nullptr, &new_pipeline) != VK_SUCCESS) {
        fmt::println("failed to create pipeline");
        return VK_NULL_HANDLE;  // failed to create graphics pipeline
    } else {
        return new_pipeline;
    }
}

void PipelineBuilder::setShaders(VkShaderModule vertex_shader,
                                  VkShaderModule fragment_shader) {
    shader_stages.clear();

    shader_stages.push_back(vkinit::pipelineShaderStageCreateInfo(
            VK_SHADER_STAGE_VERTEX_BIT, vertex_shader));

    shader_stages.push_back(vkinit::pipelineShaderStageCreateInfo(
            VK_SHADER_STAGE_FRAGMENT_BIT, fragment_shader));
}

void PipelineBuilder::setInputTopology(VkPrimitiveTopology topology) {
    input_assembly.topology = topology;
    // we are not going to use primitive restart on the entire tutorial so leave
    // it on false
    input_assembly.primitiveRestartEnable = VK_FALSE;
}

void PipelineBuilder::setPolygonMode(VkPolygonMode mode) {
    rasterizer.polygonMode = mode;
    rasterizer.lineWidth = 1.f;
}

void PipelineBuilder::setCullMode(VkCullModeFlags cull_mode,
                                    VkFrontFace front_face) {
    rasterizer.cullMode = cull_mode;
    rasterizer.frontFace = front_face;
}

void PipelineBuilder::setMultisamplingNone() {
    multisampling.sampleShadingEnable = VK_FALSE;
    // multisampling defaulted to no multisampling (1 sample per pixel)
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    // no alpha to coverage either
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;
}

void PipelineBuilder::disableBlending() {
    // default write mask
    color_blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    // no blending
    color_blend_attachment.blendEnable = VK_FALSE;
}

void PipelineBuilder::setColorAttachmentFormat(VkFormat format) {
    color_attachment_format = format;
    // connect the format to the renderInfo  structure
    render_info.colorAttachmentCount = 1;
    render_info.pColorAttachmentFormats = &color_attachment_format;
}

void PipelineBuilder::setDepthFormat(VkFormat format) {
    render_info.depthAttachmentFormat = format;
}

void PipelineBuilder::disableDepthtest() {
    depth_stencil.depthTestEnable = VK_FALSE;
    depth_stencil.depthWriteEnable = VK_FALSE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_NEVER;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.stencilTestEnable = VK_FALSE;
    depth_stencil.front = {};
    depth_stencil.back = {};
    depth_stencil.minDepthBounds = 0.f;
    depth_stencil.maxDepthBounds = 1.f;
}

void PipelineBuilder::enableDepthtest(bool depth_write_enable, VkCompareOp op) {
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = depth_write_enable;
    depth_stencil.depthCompareOp = op;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.stencilTestEnable = VK_FALSE;
    depth_stencil.front = {};
    depth_stencil.back = {};
    depth_stencil.minDepthBounds = 0.f;
    depth_stencil.maxDepthBounds = 1.f;
}

void PipelineBuilder::enableBlendingAdditive() {
    color_blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_TRUE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
}

void PipelineBuilder::enableBlendingAlphablend() {
    color_blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_TRUE;
    color_blend_attachment.srcColorBlendFactor =
            VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
}