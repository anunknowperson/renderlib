#include "scene/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
    : position(glm::vec3(0.0f, 0.0f, 3.0f)),
      rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
      fov(45.0f),
      width(800.0f),
      height(600.0f),
      _direction(glm::vec3(0.0f, 0.0f, -1.0f)),
      _viewMatrix(glm::mat4(1.0f)) {}

Camera::Camera(const glm::vec3& position, float fov, float width, float height)
    : position(position),
      rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
      fov(fov),
      width(width),
      height(height),
      _direction(glm::vec3(0.0f, 0.0f, -1.0f)),
      _viewMatrix(glm::mat4(1.0f)) {}

glm::vec3 Camera::getPosition() const {
    return position;
}

glm::quat Camera::getRotation() const {
    return rotation;
}

float Camera::getFOV() const {
    return fov;
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + _direction,
                       glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::updateViewMatrix() {
    _viewMatrix = glm::lookAt(position, position + _direction,
                              glm::vec3(0.0f, 1.0f, 0.0f));
}
