#include "core/CameraController.h"

#include <glm/gtx/quaternion.hpp>

CameraController::CameraController(Camera& cam)
    : _camera(cam), _pitch(0.0f), _yaw(0.0f) {}

void CameraController::lookAt(const glm::vec3& target) {
    glm::vec3 direction = glm::normalize(target - _camera.position);
    _camera.rotation = glm::quatLookAt(direction, glm::vec3(0.0f, 1.0f, 0.0f));
    _camera.updateViewMatrix();
}

// Setters and getters
// Setters:
void CameraController::setPosition(const glm::vec3& newPosition) {
    _camera.position = newPosition;
    _camera.updateViewMatrix();
}

void CameraController::setRotation(const glm::quat& newRotation) {
    _camera.rotation = newRotation;
    _camera.updateViewMatrix();
}

void CameraController::setPitch(float newPitch) {
    _pitch = newPitch;
    _camera.updateViewMatrix();
}

void CameraController::setYaw(float newYaw) {
    _yaw = newYaw;
    _camera.updateViewMatrix();
}

void CameraController::setFOV(float newFOV) {
    _camera.fov = newFOV;
}

void CameraController::setWidth(float width) {
    _camera.width = width;
}

void CameraController::setHeight(float height) {
    _camera.height = height;
}

// Getters:
glm::vec3 CameraController::getPosition() const {
    return _camera.position;
}

glm::quat CameraController::getRotation() const {
    return _camera.rotation;
}

float CameraController::getPitch() const {
    return _pitch;
}

float CameraController::getYaw() const {
    return _yaw;
}

float CameraController::getFOV() const {
    return _camera.fov;
}

float CameraController::getWidth() const {
    return _camera.width;
}

float CameraController::getHeight() const {
    return _camera.height;
}

void CameraController::processSDLEvent(const SDL_Event& event) {
    const float moveSpeed = 0.1f;
    const float lookSpeed = 0.005f;

    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_w:
                _camera.position += glm::vec3(0.0f, 0.0f, -moveSpeed);
                break;
            case SDLK_s:
                _camera.position += glm::vec3(0.0f, 0.0f, moveSpeed);
                break;
            case SDLK_a:
                _camera.position += glm::vec3(-moveSpeed, 0.0f, 0.0f);
                break;
            case SDLK_d:
                _camera.position += glm::vec3(moveSpeed, 0.0f, 0.0f);
                break;
        }
    }

    if (event.type == SDL_MOUSEMOTION) {
        _pitch -= event.motion.yrel * lookSpeed;
        _yaw -= event.motion.xrel * lookSpeed;

        glm::quat pitchQuat =
                glm::angleAxis(_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat yawQuat = glm::angleAxis(_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        _camera.rotation = yawQuat * pitchQuat;
    }
}
