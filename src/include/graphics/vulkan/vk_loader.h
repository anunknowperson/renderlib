#pragma once

#include <cstdint>
#include <filesystem>
#include <glm/ext/matrix_float4x4.hpp>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "vk_descriptors.h"
#include "vk_types.h"

class VulkanEngine;
struct DrawContext;

struct GLTFMaterial {
    MaterialInstance data;
};

struct GeoSurface {
    uint32_t start_index;
    uint32_t count;
    std::shared_ptr<GLTFMaterial> material;
};

struct MeshAsset {
    std::string name;

    std::vector<GeoSurface> surfaces;
    GPUMeshBuffers mesh_buffers;
};

std::optional<std::vector<std::shared_ptr<MeshAsset>>> loadGltfMeshes(
        VulkanEngine* engine, const std::filesystem::path& file_path);

struct LoadedGLTF final : public IRenderable {
    LoadedGLTF() = default;

    // storage for all the data on a given glTF file
    std::unordered_map<std::string, std::shared_ptr<MeshAsset>> meshes;
    std::unordered_map<std::string, std::shared_ptr<ENode>> nodes;
    std::unordered_map<std::string, AllocatedImage> images;
    std::unordered_map<std::string, std::shared_ptr<GLTFMaterial>> materials;

    // nodes that dont have a parent, for iterating through the file in tree
    // order
    std::vector<std::shared_ptr<ENode>> top_nodes;

    std::vector<VkSampler> samplers;

    DescriptorAllocatorGrowable descriptor_pool;

    AllocatedBuffer material_data_buffer;

    VulkanEngine* creator;

    ~LoadedGLTF() {
        clearAll();
    };

    void draw(const glm::mat4& top_matrix, DrawContext& ctx);

private:
    void clearAll();
};

std::optional<std::shared_ptr<LoadedGLTF>> loadGltf(VulkanEngine* engine,
                                                    std::string_view file_path);
