#include "graphics/vulkan/RenderableGLTF.h"
#include <glm/fwd.hpp>
#include <utility>
#include <vector>
#include "graphics/vulkan/vk_types.h"

#define GLM_ENABLE_EXPERIMENTAL

RenderableGLTF::RenderableGLTF(LoadedGltfPtr gltf) : _gltf(std::move(gltf)) {}

void RenderableGLTF::Draw(const glm::mat4& topMatrix, DrawContext& ctx) {
    // create renderables from the scenenodes
    for (auto& n : _gltf->topNodes) {
        n->Draw(topMatrix, ctx);
    }
}
