#include "graphics/Graphics.h"

namespace engine::graphics {

Graphics::Graphics() {}

Graphics::~Graphics() {}

Graphics* Graphics::getInstance() {
    static Graphics singleton;
    return &singleton;
}

uint64_t Graphics::createMeshInstance() {
    std::uint64_t new_index;
    std::uint64_t new_uid;

    if (_meshes.size() != 0) {
        auto last_pair = *_meshes.rbegin();

        new_index = last_pair.second + 1;
        new_uid = last_pair.first + 1;
    } else {
        new_index = 1;
        new_uid = 0;
    }

    _meshes[new_uid] = new_index;

    // VulkanRender::instances.push_back(glm::mat4(1.0));

    return new_uid;
}

void Graphics::setMeshInstanceTransform([[maybe_unused]] uint64_t rid, [[maybe_unused]] glm::mat4 matrix) {
    // VulkanRender::instances[meshes[rid]] = matrix;
}

void Graphics::freeMeshInstance(uint64_t rid) {
    // VulkanRender::instances.erase( VulkanRender::instances.begin() +
    // meshes[rid] );

    _meshes.erase(rid);

    auto it = _meshes.find(rid);
    if (it != _meshes.end()) {
        size_t index = it->second;
        // VulkanRender::instances.erase( VulkanRender::instances.begin() +
        // index );

        _meshes.erase(it);

        for (auto& entry : _meshes) {
            if (entry.second > index) {
                entry.second--;
            }
        }
    }
}

}  // namespace engine::graphics