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