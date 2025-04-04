#pragma once

#include "scene/Camera.h"
#include <glm/glm.hpp>
#include <SDL.h>

class CameraController {
public:
    explicit CameraController(Camera& camera);

    void setPosition(const glm::vec3& position);
    glm::vec3 getPosition() const;

    void lookAt(const glm::vec3& target);

    void update();
    void processSDLEvent(SDL_Event& e);

private:
    Camera& _camera;
    bool _keyStates[SDL_NUM_SCANCODES]{};
    glm::vec2 _lastMousePos{0.0f};
    bool _mouseDown = false;
};