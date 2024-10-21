#include "core/CameraController.h"

#include <glm/gtx/quaternion.hpp>

CameraController::CameraController(Camera& cam)
    : camera(cam), pitch(0.0f), yaw(0.0f) {
    updateViewMatrix();
}

void CameraController::setPosition(const glm::vec3& newPosition) {
    camera.position = newPosition;
    updateViewMatrix();
}

void CameraController::setRotation(const glm::quat& newRotation) {
    camera.rotation = newRotation;
    updateViewMatrix();
}

void CameraController::lookAt(const glm::vec3& target) {
    glm::vec3 direction = glm::normalize(target - camera.position);
    camera.rotation = glm::quatLookAt(direction, glm::vec3(0.0f, 1.0f, 0.0f));
    updateViewMatrix();
}

void CameraController::setPitch(float newPitch) {
    pitch = newPitch;
    updateViewMatrix();
}

void CameraController::setYaw(float newYaw) {
    yaw = newYaw;
    updateViewMatrix();
}

void CameraController::setFOV(float newFOV) {
    camera.fov = newFOV;
}

glm::vec3 CameraController::getPosition() const {
    return camera.position;
}

glm::quat CameraController::getRotation() const {
    return camera.rotation;
}

float CameraController::getPitch() const {
    return pitch;
}

float CameraController::getYaw() const {
    return yaw;
}

float CameraController::getFOV() const {
    return camera.fov;
}

glm::mat4 CameraController::getViewMatrix() const {
    // Создаем матрицу вида на основе позиции и ориентации камеры
    return glm::lookAt(camera.position,
                       camera.position + glm::vec3(0.0f, 0.0f, -1.0f),
                       glm::vec3(0.0f, 1.0f, 0.0f));
}

// updateViewMatrix обновляет матрицу вида камеры на основе текущей позиции и
// ориентации

// Перемещение камеры с использованием клавиш WASD.
// Изменение ориентации камеры (вращение) при движении мыши.
// Изменение скорости движения камеры с помощью клавиш Shift или Ctrl.
void CameraController::processSDLEvent(const SDL_Event& event) {
    const float moveSpeed = 0.1f;  // Скорость движения камеры
    const float lookSpeed =
            0.005f;  // Скорость вращения камеры при движении мыши

    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_w:  // Вперед
                camera.position += glm::vec3(0.0f, 0.0f, -moveSpeed);
                break;
            case SDLK_s:  // Назад
                camera.position += glm::vec3(0.0f, 0.0f, moveSpeed);
                break;
            case SDLK_a:  // Влево
                camera.position += glm::vec3(-moveSpeed, 0.0f, 0.0f);
                break;
            case SDLK_d:  // Вправо
                camera.position += glm::vec3(moveSpeed, 0.0f, 0.0f);
                break;
        }
    }

    if (event.type == SDL_MOUSEMOTION) {
        pitch -= event.motion.yrel * lookSpeed;  // Изменяем угол наклона
        yaw -= event.motion.xrel * lookSpeed;  // Изменяем угол поворота

        glm::quat pitchQuat =
                glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat yawQuat = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        camera.rotation = yawQuat * pitchQuat;  // Обновляем вращение камеры
    }
}
