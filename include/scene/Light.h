#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


class Light {
public:
    glm::vec3 position;
    float radius;
    float strength;
    glm::vec3 direction;
    glm::quat rotation;
    glm::vec3 color;

};