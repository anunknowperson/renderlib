#include "scene/Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

Camera::Camera()
        : position(glm::vec3(0.0f, 0.0f, 3.0f)),
          direction(glm::vec3(0.0f, 0.0f, -1.0f)),
          rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
          fov(45.0f),
          width(800.0f),
          height(600.0f),
          viewMatrix(glm::mat4(1.0f)) {
    updateViewMatrix();
}

Camera::Camera(const glm::vec3& position, float fov, float width, float height)
        : position(position),
          direction(glm::vec3(0.0f, 0.0f, -1.0f)),
          rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
          fov(fov),
          width(width),
          height(height),
          viewMatrix(glm::mat4(1.0f)) {
    updateViewMatrix();
}

void Camera::lookAt(const glm::vec3& target) {
    direction = glm::normalize(target - position);
    updateViewMatrix();
}

void Camera::setPosition(const glm::vec3& newPosition) {
    position = newPosition;
    updateViewMatrix();
}

void Camera::setRotation(const glm::quat& newRotation) {
    rotation = newRotation;
    // Если необходимо обновить матрицу вида при изменении вращения, раскомментируйте:
    // updateViewMatrix();
}

glm::mat4 Camera::getViewMatrix() const {
    return viewMatrix;
}

glm::vec3 Camera::getPosition() const {
    return position;
}

void Camera::setFOV(float newFOV) {
    fov = newFOV;
    // Можно добавить логику для обновления проекционной матрицы, если это необходимо
}

void Camera::updateViewMatrix() {
    viewMatrix = glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::update() {
    // Здесь можно добавить логику для обновления состояния камеры
}

void Camera::processSDLEvent(const SDL_Event& event) {
    // Обработка событий SDL, например, движение камеры с помощью клавиатуры или мыши
    if (event.type == SDL_KEYDOWN) {
        // Обработка нажатий клавиш
    }
}
