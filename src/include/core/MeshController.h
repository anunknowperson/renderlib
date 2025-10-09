#pragma once

#include <glm/detail/type_mat4x4.hpp>
#include <string_view>

#include "scene/Mesh.h"
#include "interfaces/IModel.h"

class MeshController {
public:
    MeshController() = delete;
    explicit MeshController(IModel::Ptr model);
    ~MeshController() = default;

    void create_mesh(std::string_view file_path) const;
    void delete_mesh(Mesh::rid_t id) const;

    void set_transform(Mesh::rid_t id, glm::mat4 t) const;
    [[nodiscard]] glm::mat4 get_transform(Mesh::rid_t id) const;

    [[nodiscard]] std::vector<Mesh::rid_t> get_meshes() const;
private:
    IModel::Ptr _model;
};