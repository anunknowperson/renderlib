#include "scene/Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
    : _position(glm::vec3(0.0f, 0.0f, 3.0f)),
      _rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
      _fov(45.0f),
      _screenWidth(800.0f),
      _screenHeight(600.0f),
      _viewMatrix(glm::mat4(1.0f)) {
    updateViewMatrix();
}

Camera::Camera(const glm::vec3& position, float fov, float screenWidth, float screenHeight)
    : _position(position),
      _rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
      _fov(fov),
      _screenWidth(screenWidth),
      _screenHeight(screenHeight),
      _viewMatrix(glm::mat4(1.0f)) {
    updateViewMatrix();
}

glm::vec3 Camera::getPosition() const {
    return _position;
}

void Camera::setPosition(const glm::vec3& position) {
    _position = position;
    updateViewMatrix(); // Автоматическое обновление
}

glm::quat Camera::getRotation() const {
    return _rotation;
}

void Camera::setRotation(const glm::quat& rotation) {
    _rotation = rotation;
    updateViewMatrix(); // Автоматическое обновление
}

float Camera::getFOV() const {
    return _fov;
}

void Camera::setFOV(float fov) {
    _fov = fov;
}

float Camera::getScreenWidth() const {
    return _screenWidth;
}

void Camera::setScreenWidth(float screenWidth) {
    _screenWidth = screenWidth;
}

float Camera::getScreenHeight() const {
    return _screenHeight;
}

void Camera::setScreenHeight(float screenHeight) {
    _screenHeight = screenHeight;
}

float Camera::getPitch() const {
    return glm::degrees(glm::pitch(_rotation));
}

void Camera::setPitch(float pitch) {
    float currentYaw = glm::yaw(_rotation);
    _rotation = glm::quat(glm::vec3(glm::radians(pitch), currentYaw, 0.0f));
    updateViewMatrix();
}

void Camera::rotatePitch(float angle) {
    glm::quat pitchQuat = glm::angleAxis(glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
    _rotation = pitchQuat * _rotation;
    updateViewMatrix();
}

glm::mat4 Camera::getViewMatrix() const {
    return _viewMatrix;
}

void Camera::updateViewMatrix() {
    // Вычисляем направление взгляда на основе кватерниона
    glm::vec3 forward = _rotation * glm::vec3(0.0f, 0.0f, -1.0f);
    _viewMatrix = glm::lookAt(
            _position,
            _position + forward,
            glm::vec3(0.0f, 1.0f, 0.0f)  // Вектор "вверх"
    );
}