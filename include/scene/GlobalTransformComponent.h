#pragma once
#include "flecs.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/orthonormalize.hpp"
#include "../src/core/logging.h"

struct GlobalTransform
{
    glm::f64mat4 TransformMatrix;
};