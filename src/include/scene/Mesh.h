#pragma once

#include <filesystem>

#include "graphics/vulkan/vk_descriptors.h"

class VulkanEngine;

namespace Mesh {
// Mesh(std::string filePath);
// ~Mesh();
//
// void set_transform(glm::mat4 t);
// glm::mat4 get_transform(glm::mat4 t);
using rid_t = int64_t;

// ид меша
// матрица трансформации
// glm::mat4 transform;

namespace GLTF {
struct GLTFMaterial {
    MaterialInstance data;
};

struct GeoSurface {
    uint32_t startIndex;
    uint32_t count;
    std::shared_ptr<GLTFMaterial> material;
};

struct MeshAsset {
    std::string name;

    std::vector<GeoSurface> surfaces;
    GPUMeshBuffers meshBuffers;
};

struct LoadedGLTF {
    // storage for all the data on a given glTF file
    std::unordered_map<std::string, std::shared_ptr<MeshAsset>> meshes;
    std::unordered_map<std::string, std::shared_ptr<ENode>> nodes;
    std::unordered_map<std::string, AllocatedImage> images;
    std::unordered_map<std::string, std::shared_ptr<GLTFMaterial>> materials;

    // nodes that dont have a parent, for iterating through the file in tree
    // order
    std::vector<std::shared_ptr<ENode>> topNodes;

    std::vector<VkSampler> samplers;

    DescriptorAllocatorGrowable descriptorPool;

    AllocatedBuffer materialDataBuffer;

    VulkanEngine* creator;
};
}
};