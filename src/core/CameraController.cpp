#include "core/CameraController.h"

#include "iostream"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/quaternion.hpp>

CameraController::CameraController(Camera& camera) : _camera(camera) {
    glm::vec3 euler = glm::eulerAngles(_camera.getRotation());
    _pitch = euler.x;
    _yaw = euler.y;
}

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

void CameraController::processSDLEvent(const SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        _yaw   -= event.motion.xrel * _mouseSensitivity;
        _pitch -= event.motion.yrel * _mouseSensitivity;

        // Ограничение угла pitch
        _pitch = glm::clamp(_pitch, glm::radians(-89.0f), glm::radians(89.0f));

        // Вычисляем новый кватернион на основе yaw и pitch (roll фиксирован равным 0)
        _camera.setRotation(glm::quat(glm::vec3(_pitch, _yaw, 0.0f)));
    }

    // Обработка клавиатуры остаётся без изменений
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        glm::vec3 moveDir(0);
        switch (event.key.keysym.sym) {
            case SDLK_w: moveDir.z = -1; break;
            case SDLK_s: moveDir.z = 1; break;
            case SDLK_a: moveDir.x = -1; break;
            case SDLK_d: moveDir.x = 1; break;
        }

        glm::mat4 rotation = _camera.getRotationMatrix();
        glm::vec3 worldMove = glm::vec3(rotation * glm::vec4(moveDir, 0));

        if (event.type == SDL_KEYDOWN) {
            _camera.setPosition(_camera.getPosition() + worldMove * _moveSpeed);
        }
    }
}
