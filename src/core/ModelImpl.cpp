#include "core/ModelImpl.h"

#include <SDL_video.h>

#include "core/Mesh.h"

ModelImpl::~ModelImpl() {
    _engine.cleanup();
}

ModelImpl::ModelImpl() : _cameraController(_camera) {}

void ModelImpl::registerWindow(SDL_Window *window) {
    auto camera = std::make_unique<Camera>();
    _engine.setMainCamera(std::move(camera));
    _engine.init(window);
}

void ModelImpl::updateVulkan() {
    _engine.update();
}

Camera* ModelImpl::getCamera() {
    return _engine.getMainCamera();
}

void ModelImpl::createMesh(std::string name) {
    const auto mesh = std::make_shared<Mesh>("/basicmesh.glb");

    mesh->set_transform(glm::mat4(1.0f));

    _meshes[name] = mesh;
}

void ModelImpl::setMeshTransform(std::string name, glm::mat4x4 transform) {
    _meshes[name]->set_transform(transform);
}

/*        : _dev { openDevice() }
        , _brightness { collectBrightness(_dev) }
        , _rgb { collectRGB(_dev) }
{*/

CameraController *ModelImpl::getCameraController() {
    return &_cameraController;
}