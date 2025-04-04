#define GLM_ENABLE_EXPERIMENTAL

#include "core/CameraController.h"
#include <glm/gtx/quaternion.hpp>

CameraController::CameraController(Camera& camera) : _camera(camera) {}

void CameraController::setPosition(const glm::vec3& position) {
    _camera.setPosition(position);
}

glm::vec3 CameraController::getPosition() const {
    return _camera.getPosition();
}

void CameraController::lookAt(const glm::vec3& target) {
    glm::vec3 direction = glm::normalize(target - _camera.getPosition());
    glm::quat rotation = glm::quatLookAt(direction, glm::vec3(0, 1, 0));
    _camera.setRotation(rotation);
}

void CameraController::update() {
    glm::vec3 moveDir(0.0f);
    float moveSpeed = 0.1f;

    if (_keyStates[SDL_SCANCODE_W]) moveDir.z -= 1.0f;
    if (_keyStates[SDL_SCANCODE_S]) moveDir.z += 1.0f;
    if (_keyStates[SDL_SCANCODE_A]) moveDir.x -= 1.0f;
    if (_keyStates[SDL_SCANCODE_D]) moveDir.x += 1.0f;
    if (_keyStates[SDL_SCANCODE_Q]) moveDir.y -= 1.0f;
    if (_keyStates[SDL_SCANCODE_E]) moveDir.y += 1.0f;

    if (glm::length(moveDir) > 0.01f) {
        glm::vec3 movement = _camera.getRotationMatrix() * glm::vec4(moveDir, 0.0f);
        _camera.setPosition(_camera.getPosition() + movement * moveSpeed);
    }
}

void CameraController::processSDLEvent(SDL_Event& e) {
    switch (e.type) {
        case SDL_KEYDOWN:
            _keyStates[e.key.keysym.scancode] = true;
        break;
        case SDL_KEYUP:
            _keyStates[e.key.keysym.scancode] = false;
        break;
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_RIGHT) {
                _mouseDown = true;
                _lastMousePos = glm::vec2(e.button.x, e.button.y);
            }
        break;
        case SDL_MOUSEBUTTONUP:
            if (e.button.button == SDL_BUTTON_RIGHT) {
                _mouseDown = false;
            }
        break;
        case SDL_MOUSEMOTION:
            if (_mouseDown) {
                glm::vec2 mousePos(e.motion.x, e.motion.y);
                glm::vec2 delta = mousePos - _lastMousePos;
                _lastMousePos = mousePos;

                float sensitivity = 0.1f;
                glm::vec3 angles = _camera.getEulerAngles();
                angles.y -= delta.x * sensitivity;
                angles.x -= delta.y * sensitivity;
                angles.x = glm::clamp(angles.x, -89.0f, 89.0f);

                _camera.setEulerAngles(angles);
            }
        break;
    }
}