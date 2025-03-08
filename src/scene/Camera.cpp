#define GLM_ENABLE_EXPERIMENTAL
#include "scene/Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Camera::Camera() {
    updateProjectionMatrix();
    updateViewMatrix();
}

Camera::Camera(const glm::vec3& position, float fov, float screenWidth, float screenHeight)
    : _position(position),
      _fov(fov),
      _screenWidth(screenWidth),
      _screenHeight(screenHeight) {
    updateProjectionMatrix();
    updateViewMatrix();
}

void Camera::update(float deltaTime) {
    _position += _velocity * deltaTime;
    updateViewMatrix();
}

glm::mat4 Camera::getViewMatrix() const {
    return _viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() const {
    return _projectionMatrix;
}

glm::mat4 Camera::getRotationMatrix() const {
    return glm::mat4_cast(_rotation);
}

void Camera::updateViewMatrix() {
    const glm::mat4 translation = glm::translate(glm::mat4(1.0f), _position);
    const glm::mat4 rotation = glm::mat4_cast(_rotation);
    _viewMatrix = glm::inverse(translation * rotation);
}

void Camera::updateProjectionMatrix() {
    _projectionMatrix = glm::perspective(
            glm::radians(_fov),
            _screenWidth / _screenHeight,
            _nearClip,
            _farClip
    );
    _projectionMatrix[1][1] *= -1;
}

glm::vec3 Camera::getPosition() const {
    return _position;
}

void Camera::setPosition(const glm::vec3& position) {
    _position = position;
    updateViewMatrix();
}

glm::quat Camera::getRotation() const {
    return _rotation;
}

void Camera::setRotation(const glm::quat& rotation) {
    _rotation = rotation;
    updateViewMatrix();
}

float Camera::getFOV() const {
    return _fov;
}

void Camera::setFOV(float fov) {
    _fov = fov;
    updateProjectionMatrix();
}

float Camera::getScreenWidth() const {
    return _screenWidth;
}

void Camera::setScreenWidth(float width) {
    _screenWidth = width;
    updateProjectionMatrix();
}

float Camera::getScreenHeight() const {
    return _screenHeight;
}

void Camera::setScreenHeight(float height) {
    _screenHeight = height;
    updateProjectionMatrix();
}

glm::vec3 Camera::getEulerAngles() const {
    return glm::degrees(glm::eulerAngles(_rotation));
}

void Camera::setEulerAngles(const glm::vec3& angles) {
    _rotation = glm::quat(glm::radians(angles));
    updateViewMatrix();
}