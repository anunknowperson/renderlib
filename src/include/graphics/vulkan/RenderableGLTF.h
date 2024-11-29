#pragma once

#include <filesystem>

#include "vk_types.h"
#include "scene/Mesh.h"

class RenderableGLTF : public IRenderable {
public:
    using LoadedGltfPtr = std::shared_ptr<const Mesh::GLTF::LoadedGLTF>;
    explicit RenderableGLTF(LoadedGltfPtr gltf);

    void Draw(const glm::mat4& topMatrix, DrawContext& ctx) final;

private:
    std::shared_ptr<const Mesh::GLTF::LoadedGLTF> _gltf;
};


