#pragma once

#include "graphics/vulkan/vk_types.h"
#include "scene/Mesh.h"

#include <glm/glm.hpp>

struct MeshNode final : ENode {
    std::shared_ptr<Mesh::GLTF::MeshAsset> mesh;

    void Draw(const glm::mat4& topMatrix, DrawContext& ctx) override;
};
