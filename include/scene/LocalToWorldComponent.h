#pragma once
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtx/quaternion.hpp"

struct LocalToWorldComponent {
    glm::mat4 Matrix;
    glm::vec3 Right = glm::vec3(Matrix[0][0], Matrix[0][1], Matrix[0][2]);
    glm::vec3 Up = glm::vec3(Matrix[1][0], Matrix[1][1], Matrix[1][2]);
    glm::vec3 Forward = glm::vec3(Matrix[2][0], Matrix[2][1], Matrix[2][2]);
    glm::vec3 Position = glm::vec3(Matrix[3][0], Matrix[3][1], Matrix[3][2]);
    glm::quat Rotation = glm::quat_cast(Matrix);
};

