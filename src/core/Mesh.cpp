#include "core/Mesh.h"

Mesh::Mesh(const std::string &file_path) {
    VulkanEngine &engine = VulkanEngine::get();

    _rid = engine.registerMesh(file_path);
}

Mesh::~Mesh() {
    VulkanEngine &engine = VulkanEngine::get();

    engine.unregisterMesh(_rid);
}

void Mesh::setTransform(glm::mat4 t) {
    VulkanEngine &engine = VulkanEngine::get();

    _transform = t;

    engine.setMeshTransform(_rid, t);
}

glm::mat4 Mesh::getTransform() {
    return _transform;
}
