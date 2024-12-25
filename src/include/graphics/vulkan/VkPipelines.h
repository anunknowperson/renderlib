#pragma once
#include "VkTypes.h"

namespace vkutil {

bool loadShaderModule(const char* file_path, VkDevice device,
                        VkShaderModule* out_shader_module);

};

class PipelineBuilder {
public:
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages;

    VkPipelineInputAssemblyStateCreateInfo input_assembly;
    VkPipelineRasterizationStateCreateInfo rasterizer;
    VkPipelineColorBlendAttachmentState color_blend_attachment;
    VkPipelineMultisampleStateCreateInfo multisampling;
    VkPipelineLayout pipeline_layout;
    VkPipelineDepthStencilStateCreateInfo depth_stencil;
    VkPipelineRenderingCreateInfo render_info;
    VkFormat color_attachment_format;

    PipelineBuilder() {
        clear();
    }

    void clear();

    VkPipeline buildPipeline(VkDevice device);

    void setShaders(VkShaderModule vertex_shader,
                     VkShaderModule fragment_shader);

    void setInputTopology(VkPrimitiveTopology topology);

    void setPolygonMode(VkPolygonMode mode);

    void setCullMode(VkCullModeFlags cull_mode, VkFrontFace front_face);

    void setMultisamplingNone();

    void disableBlending();

    void setColorAttachmentFormat(VkFormat format);

    void setDepthFormat(VkFormat format);

    void disableDepthtest();

    void enableDepthtest(bool depth_write_enable, VkCompareOp op);

    void enableBlendingAdditive();

    void enableBlendingAlphablend();
};
