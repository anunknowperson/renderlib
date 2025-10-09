#pragma once

#include <filesystem>
#include <unordered_map>

#include "vk_descriptors.h"
#include "vk_types.h"
#include "scene/Mesh.h"

// struct GLTFMaterial {
//     MaterialInstance data;
// };
//
// struct GeoSurface {
//     uint32_t startIndex;
//     uint32_t count;
//     std::shared_ptr<GLTFMaterial> material;
// };
//
// struct MeshAsset {
//     std::string name;
//
//     std::vector<GeoSurface> surfaces;
//     GPUMeshBuffers meshBuffers;
// };

// forward declaration
// class VulkanEngine;

// в vk_engine.cpp
// std::optional<std::vector<std::shared_ptr<MeshAsset>>> loadGltfMeshes(
//         VulkanEngine* engine, std::filesystem::path filePath);

class RenderableGLTF : public IRenderable {
public:
    // // storage for all the data on a given glTF file
    // std::unordered_map<std::string, std::shared_ptr<MeshAsset>> meshes;
    // std::unordered_map<std::string, std::shared_ptr<ENode>> nodes;
    // std::unordered_map<std::string, AllocatedImage> images;
    // std::unordered_map<std::string, std::shared_ptr<GLTFMaterial>> materials;
    //
    // // nodes that dont have a parent, for iterating through the file in tree
    // // order
    // std::vector<std::shared_ptr<ENode>> topNodes;
    //
    // std::vector<VkSampler> samplers;
    //
    // DescriptorAllocatorGrowable descriptorPool;
    //
    // AllocatedBuffer materialDataBuffer;
    //
    // VulkanEngine* creator;
    using LoadedGltfPtr = std::shared_ptr<const Mesh::GLTF::LoadedGLTF>;
    explicit RenderableGLTF(LoadedGltfPtr gltf);
    virtual ~RenderableGLTF() {
        clearAll();
    };

    void Draw(const glm::mat4& topMatrix, DrawContext& ctx) final;

private:
    void clearAll();
    std::shared_ptr<const Mesh::GLTF::LoadedGLTF> _gltf;
};


