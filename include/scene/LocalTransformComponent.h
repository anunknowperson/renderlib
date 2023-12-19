#pragma once
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/orthonormalize.hpp"
#include "../src/core/logging.h"

struct LocalTransform
{
    glm::f64vec3 position = glm::vec3(0.0f);
    glm::f64quat rotation = glm::quat(1.0f, 1.0f, 0.0f, 0.0f);
    glm::float64 scale = 1.0f;
};