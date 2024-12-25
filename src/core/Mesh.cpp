#include "core/Mesh.h"

Mesh::Mesh(std::string filePath) {
    VulkanEngine &engine = VulkanEngine::get();

    _rid = engine.registerMesh(filePath);
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
