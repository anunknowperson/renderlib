#include "core/MeshController.h"

#include <graphics/vulkan/vk_engine.h>

#include "scene/Mesh.h"

MeshController::MeshController(IModel::Ptr model) : _model(std::move(model)) {}

void MeshController::create_mesh(std::string_view file_path) const {
        //TODO: посмотреть, можно ли _engine засунуть в private
    VulkanEngine& engine = _model->get_engine();

    _model->createMesh(engine, file_path);

    // return rid;
}

void MeshController::delete_mesh(Mesh::rid_t id) const {
    _model->delete_mesh(id);
}

void MeshController::set_transform(Mesh::rid_t id, glm::mat4 t) const {
    _model->setMeshTransform(id, t);
}

glm::mat4 MeshController::get_transform(Mesh::rid_t id) const {
    const auto transform = _model->get_mesh_transform(id);
    return transform;
}
