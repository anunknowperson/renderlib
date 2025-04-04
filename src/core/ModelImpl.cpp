#include "core/ModelImpl.h"

#include <SDL_video.h>
#include <filesystem>
#include <iostream>

#include "core/Mesh.h"

ModelImpl::~ModelImpl() {
    _engine.cleanup();
}

ModelImpl::ModelImpl() = default;

void ModelImpl::registerWindow(SDL_Window *window) {
    _engine.mainCamera = &_camera;
    _engine.init(window);
}

void ModelImpl::updateVulkan() {
    _engine.update();
}

Camera *ModelImpl::getCamera() {
    return &_camera;
}

void ModelImpl::createMesh(std::string name) {
    std::string filePath = "/basicmesh.glb";

    /* if (!std::filesystem::exists(filePath)) {
        std::cerr << "Warning: Mesh file not found at " << filePath
                  << std::endl;
    }*/

    const auto mesh = std::make_shared<Mesh>(filePath);

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