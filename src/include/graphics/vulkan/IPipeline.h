#pragma once

#include "vk_types.h"

class IPipeline {
public:
    virtual ~IPipeline() = default;
    
    virtual void init(VkDevice device) = 0;
    virtual void bind(VkCommandBuffer cmd) = 0;
    virtual void destroy() = 0;
    
    // Optional methods for pipeline-specific binding
    virtual VkPipeline getPipeline() const = 0;
    virtual VkPipelineLayout getLayout() const = 0;
};