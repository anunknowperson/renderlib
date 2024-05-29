#include "core/Mesh.h"


Mesh::Mesh(std::string filePath) {

    VulkanEngine &engine = VulkanEngine::Get();

    _rid = engine.registerMesh(filePath);
}

Mesh::~Mesh() {
    VulkanEngine &engine = VulkanEngine::Get();

    engine.unregisterMesh(_rid);
}

void Mesh::set_transform(glm::mat4 t) {
    VulkanEngine &engine = VulkanEngine::Get();

    _transform = t;

    engine.setMeshTransform(_rid, t);
}

glm::mat4 Mesh::get_transform(glm::mat4 t) {
    return _transform;
}
