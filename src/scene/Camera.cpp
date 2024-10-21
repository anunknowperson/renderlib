#include "scene/Camera.h"

Camera::Camera()
    : position(glm::vec3(0.0f, 0.0f, 3.0f)),
      rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
      fov(45.0f),
      width(800.0f),
      height(600.0f),
      direction(glm::vec3(0.0f, 0.0f, -1.0f)),
      viewMatrix(glm::mat4(1.0f)) {}

Camera::Camera(const glm::vec3& position, float fov, float width, float height)
    : position(position),
      rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
      fov(fov),
      width(width),
      height(height),
      direction(glm::vec3(0.0f, 0.0f, -1.0f)),
      viewMatrix(glm::mat4(1.0f)) {}

glm::vec3 Camera::getPosition() const {
    return position;
}

glm::quat Camera::getRotation() const {
    return rotation;
}

float Camera::getFOV() const {
    return fov;
}
