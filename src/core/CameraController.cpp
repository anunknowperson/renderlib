#include "core/CameraController.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

CameraController::CameraController(Camera& cam)
    : _camera(cam), _pitch(0.0f), _yaw(0.0f) {}

void CameraController::setPosition(const glm::vec3& newPosition) {
    _camera._position = newPosition;
    _camera.updateViewMatrix();
}

glm::vec3 CameraController::getPosition() const {
    return _camera._position;
}

void CameraController::setRotation(const glm::quat& newRotation) {
    _camera._rotation = newRotation;
    _camera.updateViewMatrix();
}

glm::quat CameraController::getRotation() const {
    return _camera._rotation;
}

void CameraController::setPitch(float newPitch) {
    _pitch = newPitch;
    _camera.updateViewMatrix();
}

float CameraController::getPitch() const {
    return _pitch;
}

void CameraController::setYaw(float newYaw) {
    _yaw = newYaw;
    _camera.updateViewMatrix();
}

float CameraController::getYaw() const {
    return _yaw;
}

void CameraController::lookAt(const glm::vec3& target) {
    glm::vec3 direction = glm::normalize(target - _camera._position);
    _camera._rotation = glm::quatLookAt(direction, glm::vec3(0.0f, 1.0f, 0.0f));
    _camera.updateViewMatrix();
}

void CameraController::setFOV(float newFOV) {
    _camera._fov = newFOV;
}

float CameraController::getFOV() const {
    return _camera._fov;
}

void CameraController::setScreenWidth(float screenWidth) {
    _camera._screenWidth = screenWidth;
}

float CameraController::getScreenWidth() const {
    return _camera._screenWidth;
}

void CameraController::setScreenHeight(float screenHeight) {
    _camera._screenHeight = screenHeight;
}

float CameraController::getScreenHeight() const {
    return _camera._screenHeight;
}

void CameraController::update() {
    _camera.updateViewMatrix();  // Обновляем матрицу вида
}

void CameraController::processSDLEvent(const SDL_Event& event) {
    const float moveSpeed = 0.1f;
    const float lookSpeed = 0.005f;

    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_w:
                _camera._position += glm::vec3(0.0f, 0.0f, -moveSpeed);
                break;
            case SDLK_s:
                _camera._position += glm::vec3(0.0f, 0.0f, moveSpeed);
                break;
            case SDLK_a:
                _camera._position += glm::vec3(-moveSpeed, 0.0f, 0.0f);
                break;
            case SDLK_d:
                _camera._position += glm::vec3(moveSpeed, 0.0f, 0.0f);
                break;
        }
    }

    if (event.type == SDL_MOUSEMOTION) {
        _pitch -= event.motion.yrel * lookSpeed;
        _yaw -= event.motion.xrel * lookSpeed;

        glm::quat pitchQuat =
                glm::angleAxis(_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat yawQuat = glm::angleAxis(_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        _camera._rotation = yawQuat * pitchQuat;
    }
}