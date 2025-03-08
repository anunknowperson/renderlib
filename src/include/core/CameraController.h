#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "scene/Camera.h"

class CameraController {
public:
    explicit CameraController(Camera& camera);

    void setPosition(const glm::vec3& position);
    glm::vec3 getPosition() const;
    void lookAt(const glm::vec3& target);
    void processSDLEvent(const SDL_Event& event);

private:
    Camera& _camera;
    float _moveSpeed = 0.1f; // Скорость перемещения
    float _mouseSensitivity = 0.001f; // Чувствительность мыши
    float _yaw = 0.f;
    float _pitch = 0.f;
};