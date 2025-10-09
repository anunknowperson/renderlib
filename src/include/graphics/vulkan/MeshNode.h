#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <memory>
#include "graphics/vulkan/vk_types.h"

namespace Mesh::GLTF {
struct MeshAsset;
}

struct MeshNode final : ENode {
    std::shared_ptr<Mesh::GLTF::MeshAsset> mesh;

    void Draw(const glm::mat4& topMatrix, DrawContext& ctx) override;
};
