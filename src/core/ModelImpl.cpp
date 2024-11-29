#include "core/ModelImpl.h"

#include <cassert>
#include <cstring>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
ModelImpl::~ModelImpl() {
    _engine.cleanup();
}

ModelImpl::ModelImpl() = default;

void ModelImpl::registerWindow(struct SDL_Window* window) {
    _engine.mainCamera = &_camera;
    _engine.init(window);
}

VulkanEngine& ModelImpl::get_engine() {
    assert(_engine._isInitialized);
    return _engine;
}

Camera* ModelImpl::getCamera() {
    return &_camera;
}

#include "core/config.h"
#include "scene/LoaderGLTF.h"

Mesh::rid_t registerMesh(
        VulkanEngine& engine,
        ModelImpl::MeshMap& meshes,
        std::string_view filePath) {
    std::random_device rd;

    // Use the Mersenne Twister engine for high-quality random numbers
    std::mt19937_64 generator(rd());

    // Create a uniform distribution for int64_t
    std::uniform_int_distribution<Mesh::rid_t> distribution;

    // Generate and print a random int64_t value
    const Mesh::rid_t random_rid_t = distribution(generator);

    std::string structurePath = {std::string(ASSETS_DIR) +
                                 std::string(filePath)};
    auto structureFile = LoaderGLTF::loadGLTF(engine, structurePath);

    assert(structureFile.has_value());

    engine.loadedScenes["structure"] = *structureFile;

    meshes[random_rid_t] = {structureFile.value(), glm::mat4(1.)};

    return random_rid_t;
}

void ModelImpl::createMesh(VulkanEngine& engine, std::string_view file_path) {
    assert(_engine._isInitialized);
    const Mesh::rid_t rid = registerMesh(engine, _meshes, file_path);
    _meshes[rid].transform = 1.;
// TODO вернуть возвращаемое значение
    // return rid;
}

void ModelImpl::setMeshTransform(Mesh::rid_t rid, glm::mat4x4 transform) {
    _meshes[rid].transform = transform;
}

glm::mat4 ModelImpl::get_mesh_transform(Mesh::rid_t rid) {
    return _meshes[rid].transform;
}

void ModelImpl::delete_mesh(Mesh::rid_t rid) {
    _meshes.erase(rid);
}

const ModelImpl::MeshMap& ModelImpl::get_meshes() {
    return _meshes;
}

