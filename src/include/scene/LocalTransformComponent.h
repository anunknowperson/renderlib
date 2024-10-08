#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/orthonormalize.hpp"
#include "core/Logging.h"

/** @brief A component that stores coordinates in 3D relative to the parent entity.
 *
 * @details This component is used to store the position, rotation and scale of an entity relative to its parent.
 * */

struct LocalTransform
{
    glm::f64vec3 position = glm::vec3(0.0f);

    glm::f64quat rotation = glm::quat(1.0f, 1.0f, 0.0f, 0.0f);

    glm::float64 scale = 1.0f;
};