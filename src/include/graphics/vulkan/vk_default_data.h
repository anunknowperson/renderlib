#pragma once

#include "vk_types.h"
#include <memory>
#include <vector>

class VulkanEngine;
struct MeshAsset;

class DefaultDataManager {
public:
    DefaultDataManager(VulkanEngine* engine);
    
    void init_default_data();

private:
    VulkanEngine* _engine;
};