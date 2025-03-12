#pragma once


#include <vulkan/vulkan.h>
#include <vector>
#include <functional>



class CommandBuffers {
public:

    void immediate_submit(
            std::function<void(VkCommandBuffer cmd)>&& function) const;

    void init_commands();

private:

};