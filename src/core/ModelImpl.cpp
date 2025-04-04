#include "core/ModelImpl.h"

#include <SDL_video.h>

#include "core/Mesh.h"

ModelImpl::~ModelImpl() {
    delete _cameraController;
    _engine.cleanup();
}

ModelImpl::ModelImpl() {
    _camera = Camera(
        glm::vec3(0.0f, 0.0f, 5.0f),
        70.0f,
        800.0f,
        600.0f,
        0.1f,
        1000.0f
    );

    _cameraController = new CameraController(_camera);
}

void ModelImpl::registerWindow(SDL_Window *window) {
    _engine.mainCamera = &_camera;
    _engine.cameraController = _cameraController;
    _engine.init(window);
}

void ModelImpl::updateVulkan() {
    _engine.update();
}

Camera *ModelImpl::getCamera() {
    return &_camera;
}

CameraController* ModelImpl::getCameraController() const {
    return _cameraController;
}

void ModelImpl::createMesh(std::string name) {
    const auto mesh = std::make_shared<Mesh>("/basicmesh.glb");
    mesh->set_transform(glm::mat4(1.0f));
    _meshes[name] = mesh;
}

void ModelImpl::setMeshTransform(std::string name, glm::mat4x4 transform) {
    _meshes[name]->set_transform(transform);
}