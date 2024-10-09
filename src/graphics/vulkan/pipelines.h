

#include "vk_images.h"
#include "vk_initializers.h"
#include "vk_types.h"
#include "vk_pipelines.h"


class Pipelines {
public:
    
    VkPipelineLayout trianglePipelineLayout;
    VkPipeline trianglePipeline;

    VkPipelineLayout meshPipelineLayout;
    VkPipeline meshPipeline;

    void init(VkDevice device, VkDescriptorSetLayout singleImageDescriptorLayout, AllocatedImage drawImage);
    void destroy();

private:
    VkDevice _device;
    VkDescriptorSetLayout _singleImageDescriptorLayout;
    AllocatedImage _drawImage;

    void init_mesh_pipeline();
    void init_triangle_pipeline();

};
