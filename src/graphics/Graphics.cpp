#include "graphics/Graphics.h"

namespace engine::graphics {
Graphics* Graphics::getInstance() {
    static Graphics singleton;
    return &singleton;
}

uint64_t Graphics::createMeshInstance() {
    std::uint64_t new_index;
    std::uint64_t new_uid;

    if (!meshes.empty()) {
        auto lastPair = *meshes.rbegin();

        new_index = lastPair.second + 1;
        new_uid = lastPair.first + 1;
    } else {
        new_index = 1;
        new_uid = 0;
    }

    meshes[new_uid] = new_index;

    // VulkanRender::instances.push_back(glm::mat4(1.0));

    return new_uid;
}

void Graphics::setMeshInstanceTransform([[maybe_unused]] std::uint64_t rid,
                                           [[maybe_unused]] glm::mat4 matrix) {
    // VulkanRender::instances[meshes[rid]] = matrix;
}

void Graphics::freeMeshInstance(std::uint64_t rid) {
    // VulkanRender::instances.erase( VulkanRender::instances.begin() +
    // meshes[rid] );

    meshes.erase(rid);

    auto it = meshes.find(rid);
    if (it != meshes.end()) {
        const size_t index = it->second;
        // VulkanRender::instances.erase( VulkanRender::instances.begin() +
        // index );

        meshes.erase(it);

        for (auto& entry : meshes) {
            if (entry.second > index) {
                entry.second--;
            }
        }
    }
}

}  // namespace engine::graphics