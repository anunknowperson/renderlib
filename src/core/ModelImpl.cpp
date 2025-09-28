#include "core/ModelImpl.h"
#include "core/Mesh.h"
#include "graphics/vulkan/vk_engine.h"

ModelImpl::ModelImpl()
    : _camera(), _engine(std::make_shared<VulkanEngine>(_camera)) {};

void ModelImpl::updateVulkan() {
    _engine->update();
}

Camera *ModelImpl::getCamera() {
    return &_camera;
}

void ModelImpl::createMesh(std::string name) {
    const auto mesh = std::make_shared<Mesh>("/basicmesh.glb");

    mesh->set_transform(glm::mat4(1.0f));

    _meshes[name] = mesh;
}

void ModelImpl::setMeshTransform(std::string name, glm::mat4x4 transform) {
    _meshes[name]->set_transform(transform);
}
