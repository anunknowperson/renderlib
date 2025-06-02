// Mesh.cpp
#include "core/Mesh.h"

Mesh::Mesh(const std::string &filePath) : _currentModelPath(filePath) {
    VulkanEngine &engine = VulkanEngine::Get();
    _rid = engine.registerMesh(filePath);
}

Mesh::~Mesh() {
    remove_model();
}

void Mesh::set_model(const std::string &filePath) {
    VulkanEngine &engine = VulkanEngine::Get();

    // Unregister current model if exists
    if (!_currentModelPath.empty()) {
        engine.unregisterMesh(_rid);
    }

    // Register new model
    _currentModelPath = filePath;
    _rid = engine.registerMesh(filePath);

    // Reapply transform to new model
    engine.setMeshTransform(_rid, _transform);
}

void Mesh::remove_model() {
    if (!_currentModelPath.empty()) {
        VulkanEngine &engine = VulkanEngine::Get();
        engine.unregisterMesh(_rid);
        _currentModelPath.clear();
    }
}


void Mesh::set_transform(glm::mat4 t) {
    VulkanEngine &engine = VulkanEngine::Get();

    _transform = t;

    engine.setMeshTransform(_rid, t);
}

glm::mat4 Mesh::get_transform() {
    return _transform;
}
