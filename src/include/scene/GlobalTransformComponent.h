#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "flecs.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/orthonormalize.hpp"
#include "core/Logging.h"

/** @brief A component that stores global coordinates in 3D.
 *
 * @details This component is used to store the position, rotation and scale of an entity in global coordinates.
 * */

struct GlobalTransform
{
    glm::f64mat4 TransformMatrix;
};