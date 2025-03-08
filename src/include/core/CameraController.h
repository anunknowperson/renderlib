#pragma once
#include "scene/Camera.h"
#include <SDL2/SDL_events.h>
#include <glm/glm.hpp>

class CameraController {
public:
    explicit CameraController(Camera& camera);

    void setPosition(const glm::vec3& position);
    glm::vec3 getPosition() const;

    void lookAt(const glm::vec3& target);
    void processSDLEvent(const SDL_Event& event);
    void update(float deltaTime);

private:
    Camera& _camera;
    float _pitch{0.0f};
    float _yaw{0.0f};
    float _mouseSensitivity{0.1f};
    float _moveSpeed{5.0f};
    glm::vec3 _velocity{0.0f};
    void updateCameraOrientation();
};