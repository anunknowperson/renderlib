#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "core/Logging.h"
#include "flecs.h"
#include "glm/gtx/orthonormalize.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

/** @brief A component that stores global coordinates in 3D.
 *
 * @details This component is used to store the position, rotation and scale of
 * an entity in global coordinates.
 * */

struct GlobalTransform {
    glm::f64mat4 transform_matrix;
};