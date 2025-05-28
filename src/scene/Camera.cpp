#define GLM_ENABLE_EXPERIMENTAL
#include "scene/Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

BaseCamera::BaseCamera(const glm::vec3& position, float nearClip, float farClip)
    : _position(position), _nearClip(nearClip), _farClip(farClip) {}

glm::mat4 BaseCamera::getViewMatrix() const {
    if (_viewDirty) {
        updateViewMatrix();
        _viewDirty = false;
    }
    return _viewMatrix;
}

glm::mat4 BaseCamera::getProjectionMatrix() const {
    if (_projDirty) {
        updateProjectionMatrix();
        _projDirty = false;
    }
    return _projectionMatrix;
}

glm::vec3 BaseCamera::getPosition() const {
    return _position;
}

void BaseCamera::setPosition(const glm::vec3& position) {
    _position = position;
    _viewDirty = true;
}

glm::quat BaseCamera::getRotation() const {
    return _rotation;
}

void BaseCamera::setRotation(const glm::quat& rotation) {
    _rotation = rotation;
    _viewDirty = true;
}

glm::vec3 BaseCamera::getEulerAngles() const {
    return glm::degrees(glm::eulerAngles(_rotation));
}

void BaseCamera::setEulerAngles(const glm::vec3& angles) {
    _rotation = glm::quat(glm::radians(angles));
    _viewDirty = true;
}

void BaseCamera::updateViewMatrix() const {
    const glm::mat4 translation = glm::translate(glm::mat4(1.0f), _position);
    const glm::mat4 rotation = glm::mat4_cast(_rotation);
    _viewMatrix = glm::inverse(rotation) * glm::inverse(translation);
}

PerspectiveCamera::PerspectiveCamera(const glm::vec3& position, float fov,
                                     float aspectRatio, float nearClip, float farClip)
    : BaseCamera(position, nearClip, farClip), _fov(fov), _aspectRatio(aspectRatio) {
    _projDirty = true;
}

void PerspectiveCamera::updateViewport(float width, float height) {
    _aspectRatio = width / height;
    _projDirty = true;
}

float PerspectiveCamera::getFOV() const {
    return _fov;
}

void PerspectiveCamera::setFOV(float fov) {
    _fov = fov;
    _projDirty = true;
}

void PerspectiveCamera::updateProjectionMatrix() const {
    _projectionMatrix = glm::perspective(
        glm::radians(_fov),
        _aspectRatio,
        _nearClip,
        _farClip
    );
    _projectionMatrix[1][1] *= -1; // Flip for Vulkan
}

OrthographicCamera::OrthographicCamera(const glm::vec3& position, float size,
                                       float aspectRatio, float nearClip, float farClip)
    : BaseCamera(position, nearClip, farClip), _size(size), _aspectRatio(aspectRatio) {
    _projDirty = true;
}

void OrthographicCamera::updateViewport(float width, float height) {
    _aspectRatio = width / height;
    _projDirty = true;
}

float OrthographicCamera::getSize() const {
    return _size;
}

void OrthographicCamera::setSize(float size) {
    _size = size;
    _projDirty = true;
}

void OrthographicCamera::updateProjectionMatrix() const {
    float halfSize = _size * 0.5f;
    float halfWidth = halfSize * _aspectRatio;
    _projectionMatrix = glm::ortho(
        -halfWidth, halfWidth,
        -halfSize, halfSize,
        _nearClip, _farClip
    );
    _projectionMatrix[1][1] *= -1;
}

Camera::Camera()
    : PerspectiveCamera(glm::vec3(0.0f, 0.0f, 5.0f), 70.0f, 16.0f/9.0f, 0.1f, 1000.0f) {}

Camera::Camera(const glm::vec3& position, float fov,
               float screenWidth, float screenHeight,
               float nearClip, float farClip)
    : PerspectiveCamera(position, fov, screenWidth / screenHeight, nearClip, farClip) {}

float Camera::getScreenWidth() const {
    return _aspectRatio * getScreenHeight();
}

void Camera::setScreenWidth(float width) {
    _aspectRatio = width / getScreenHeight();
    _projDirty = true;
}

float Camera::getScreenHeight() const {
    return 1.0f;
}

void Camera::setScreenHeight(float height) {
    _aspectRatio = getScreenWidth() / height;
    _projDirty = true;
}