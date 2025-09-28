// Mesh.h
#pragma once

#include <filesystem>

#include "glm/detail/type_mat4x4.hpp"

class Mesh {
public:
    Mesh(const std::string& filePath);

    // Add model change functionality
    void set_model(const std::string& filePath);

    void set_transform(glm::mat4 t);
    glm::mat4 get_transform();

private:
    glm::mat4 _transform;
    std::string _currentModelPath;  // Track current model path
    int64_t _rid;
};