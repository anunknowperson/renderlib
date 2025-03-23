#pragma once

#include "scene/Camera.h"
#include <glm/glm.hpp>

class CameraController {
public:
    explicit CameraController(Camera& camera);

    void setPosition(const glm::vec3& position);
    glm::vec3 getPosition() const;

    void lookAt(const glm::vec3& target);

private:
    Camera& _camera;
};