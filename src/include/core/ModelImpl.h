#pragma once

#include "graphics/vulkan/vk_engine.h"
#include "interfaces/IModel.h"
#include "scene/Camera.h"
#include "scene/Mesh.h"

class ModelImpl : public IModel {
public:


    using MeshMap = std::unordered_map<Mesh::rid_t, MeshPair>;
    /*! \brief
     * throws std::runtime_error()
     */
    ModelImpl();
    ~ModelImpl() override;

    ModelImpl(const ModelImpl &) = delete;
    ModelImpl &operator=(const ModelImpl &) = delete;

    void registerWindow(struct SDL_Window *window) override;

    // Mesh::rid_t registerMesh(std::string_view file_path);
    void createMesh(VulkanEngine& engine, std::string_view file_path) final;
    void delete_mesh(Mesh::rid_t rid) final;

    void setMeshTransform(Mesh::rid_t rid, glm::mat4x4 transform) final;
    glm::mat4 get_mesh_transform(Mesh::rid_t) final;

    const MeshMap& get_meshes() final;

    Camera *getCamera() override;

    VulkanEngine& get_engine() final;

private:
    // std::unordered_map<Mesh::rid_t, std::shared_ptr<Mesh>> _meshes;

  // TODO: move meshes and transforms to one map
    MeshMap _meshes;

    VulkanEngine _engine;

    Camera _camera;
};