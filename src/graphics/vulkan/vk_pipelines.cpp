#include "graphics/vulkan/vk_pipelines.h"

#include <cstdint>
#include <fmt/base.h>
#include <fstream>
#include <spdlog/spdlog.h>
#include "core/Logging.h"
#include <filesystem>

#include "graphics/vulkan/vk_initializers.h"

bool vkutil::load_shader_module(const char* filePath, VkDevice device,
                                VkShaderModule* outShaderModule) {
    // open the file. With cursor at the end
    if (!std::filesystem::exists(filePath)) {
        LOGE("Shader file does not exist: {}", filePath);
        return false;
    }

    // Open the file in binary mode, with the cursor at the end
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        LOGE("Failed to open shader file: {}", filePath);
        return false;
    }

    // find what the size of the file is by looking up the location of the
    // cursor because the cursor is at the end, it gives the size directly in
    // bytes
    const auto fileSize = file.tellg();

    if (fileSize == -1) {
        LOGE("Failed to open file {}", filePath);
        return false;
    }

    // spirv expects the buffer to be on uint32, so make sure to reserve a int
    // vector big enough for the entire file
    std::vector<uint32_t> buffer(static_cast<uint32_t>(fileSize) /
                                 sizeof(uint32_t));

    // put file cursor at beginning
    file.seekg(0);

    // load the entire file into the buffer
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

    // now that the file is loaded into the buffer, we can close it
    file.close();

    // create a new shader module, using the buffer we loaded
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;

    // codeSize has to be in bytes, so multply the ints in the buffer by size of
    // int to know the real size of the buffer
    createInfo.codeSize = buffer.size() * sizeof(uint32_t);
    createInfo.pCode = buffer.data();

    // check that the creation goes well.
    VkShaderModule shaderModule;
    VkResult result =
            vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
    if (result != VK_SUCCESS) {
        LOGE("Error: Failed to create shader module.");
        return false;
    }
    *outShaderModule = shaderModule;
    LOGI("Shader module created successfully.");
    return true;
}

void PipelineBuilder::clear() {
    // Initialize input assembly with proper defaults
    _inputAssembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    // Initialize rasterizer with proper defaults
    _rasterizer = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f  // CRITICAL: Must be 1.0f, not 0.0f
    };

    // Initialize color blend attachment with defaults
    _colorBlendAttachment = {
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };

    // Initialize multisampling with proper defaults
    _multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,  // CRITICAL: Must be valid sample count
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE
    };

    _pipelineLayout = {};

    // Initialize depth-stencil with proper defaults
    _depthStencil = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_FALSE,
        .depthWriteEnable = VK_FALSE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 1.0f
    };

    // Initialize rendering info with proper defaults
    _renderInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .depthAttachmentFormat = VK_FORMAT_UNDEFINED
    };

    _shaderStages.clear();
}

VkPipeline PipelineBuilder::build_pipeline(VkDevice device) const {
    // Make local copies of struct members that we need to modify
    VkPipelineRenderingCreateInfo renderInfo = _renderInfo;
    VkPipelineRasterizationStateCreateInfo rasterizer = _rasterizer;
    VkPipelineMultisampleStateCreateInfo multisampling = _multisampling;
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = _inputAssembly;

    // Fix any uninitialized values
    if (renderInfo.depthAttachmentFormat == 0) {
        renderInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
    }

    // Ensure rasterizer line width is valid
    if (rasterizer.lineWidth <= 0.0f) {
        rasterizer.lineWidth = 1.0f;
    }

    // Ensure multisampling uses a valid sample count
    if (multisampling.rasterizationSamples == 0) {
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    }

    // Avoid POINT_LIST topology without PointSize in shader
    if (inputAssembly.topology == VK_PRIMITIVE_TOPOLOGY_POINT_LIST) {
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }

    // make viewport state from our stored viewport and scissor.
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    // Use dynamic viewport and scissor
    VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pDynamicStates = dynamicStates;
    dynamicState.dynamicStateCount = 2;

    // setup color blending
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext = nullptr;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &_colorBlendAttachment;

    // completely clear VertexInputStateCreateInfo
    const VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
    };

    // build the actual pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = &renderInfo;  // Use our local copy
    pipelineInfo.stageCount = static_cast<uint32_t>(_shaderStages.size());
    pipelineInfo.pStages = _shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly; // Use our local copy
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;  // Use our local copy
    pipelineInfo.pMultisampleState = &multisampling;  // Use our local copy
    pipelineInfo.pDepthStencilState = &_depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = _pipelineLayout;
    pipelineInfo.renderPass = VK_NULL_HANDLE; // We use dynamic rendering
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    VkPipeline newPipeline;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                  nullptr, &newPipeline) != VK_SUCCESS) {

        LOGE("Failed to create graphics pipeline in build_pipeline.");
        return VK_NULL_HANDLE;
    }

    return newPipeline;
}

void PipelineBuilder::set_shaders(VkShaderModule vertexShader,
                                  VkShaderModule fragmentShader) {
    _shaderStages.clear();

    _shaderStages.emplace_back(vkinit::pipeline_shader_stage_create_info(
            VK_SHADER_STAGE_VERTEX_BIT, vertexShader));

    _shaderStages.emplace_back(vkinit::pipeline_shader_stage_create_info(
            VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader));
}

void PipelineBuilder::set_input_topology(VkPrimitiveTopology topology) {
    _inputAssembly.topology = topology;

    // Avoid POINT_LIST topology which requires special shader setup
    if (topology == VK_PRIMITIVE_TOPOLOGY_POINT_LIST) {
        // Fall back to triangle list if point list is requested
        _inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }

    _inputAssembly.primitiveRestartEnable = VK_FALSE;
}

void PipelineBuilder::set_polygon_mode(VkPolygonMode mode) {
    _rasterizer.polygonMode = mode;
    _rasterizer.lineWidth = 1.0f;  // Always set to 1.0
}

void PipelineBuilder::set_cull_mode(VkCullModeFlags cullMode,
                                    VkFrontFace frontFace) {
    _rasterizer.cullMode = cullMode;
    _rasterizer.frontFace = frontFace;
}

void PipelineBuilder::set_multisampling_none() {
    _multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    _multisampling.pNext = nullptr;
    _multisampling.sampleShadingEnable = VK_FALSE;
    _multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    _multisampling.minSampleShading = 1.0f;
    _multisampling.pSampleMask = nullptr;
    _multisampling.alphaToCoverageEnable = VK_FALSE;
    _multisampling.alphaToOneEnable = VK_FALSE;
}

void PipelineBuilder::disable_blending() {
    // default write mask
    _colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    // no blending
    _colorBlendAttachment.blendEnable = VK_FALSE;
}

void PipelineBuilder::set_color_attachment_format(VkFormat format) {
    _colorAttachmentformat = format;
    // connect the format to the renderInfo  structure
    _renderInfo.colorAttachmentCount = 1;
    _renderInfo.pColorAttachmentFormats = &_colorAttachmentformat;
}

void PipelineBuilder::set_depth_format(VkFormat format) {
    // Ensure format is valid or undefined
    if (format != VK_FORMAT_D16_UNORM &&
        format != VK_FORMAT_D32_SFLOAT &&
        format != VK_FORMAT_D16_UNORM_S8_UINT &&
        format != VK_FORMAT_D24_UNORM_S8_UINT &&
        format != VK_FORMAT_D32_SFLOAT_S8_UINT &&
        format != VK_FORMAT_UNDEFINED) {
        format = VK_FORMAT_UNDEFINED;
    }

    _renderInfo.depthAttachmentFormat = format;
}

void PipelineBuilder::disable_depthtest() {
    _depthStencil.depthTestEnable = VK_FALSE;
    _depthStencil.depthWriteEnable = VK_FALSE;
    _depthStencil.depthCompareOp = VK_COMPARE_OP_NEVER;
    _depthStencil.depthBoundsTestEnable = VK_FALSE;
    _depthStencil.stencilTestEnable = VK_FALSE;
    _depthStencil.front = {};
    _depthStencil.back = {};
    _depthStencil.minDepthBounds = 0.f;
    _depthStencil.maxDepthBounds = 1.f;
}

void PipelineBuilder::enable_depthtest(bool depthWriteEnable, VkCompareOp op) {
    _depthStencil.depthTestEnable = VK_TRUE;
    _depthStencil.depthWriteEnable = depthWriteEnable;
    _depthStencil.depthCompareOp = op;
    _depthStencil.depthBoundsTestEnable = VK_FALSE;
    _depthStencil.stencilTestEnable = VK_FALSE;
    _depthStencil.front = {};
    _depthStencil.back = {};
    _depthStencil.minDepthBounds = 0.f;
    _depthStencil.maxDepthBounds = 1.f;
}

void PipelineBuilder::enable_blending_additive() {
    _colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    _colorBlendAttachment.blendEnable = VK_TRUE;
    _colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    _colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
    _colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    _colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    _colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    _colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
}

void PipelineBuilder::enable_blending_alphablend() {
    _colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    _colorBlendAttachment.blendEnable = VK_TRUE;
    _colorBlendAttachment.srcColorBlendFactor =
            VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    _colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
    _colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    _colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    _colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    _colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
}