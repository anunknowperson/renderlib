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
    // Интеграция движения
    _position += _velocity * deltaTime;
    updateViewMatrix();
}

void Camera::processSDLEvent(const SDL_Event& e) {
    // Обработка ввода (пример для клавиатуры)
    if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
        const float speed = 5.0f;
        const int state = (e.type == SDL_KEYDOWN) ? 1 : 0;

        switch(e.key.keysym.sym) {
            case SDLK_w: _velocity.z = -speed * state; break;
            case SDLK_s: _velocity.z = speed * state; break;
            case SDLK_a: _velocity.x = -speed * state; break;
            case SDLK_d: _velocity.x = speed * state; break;
        }
    }
}

// Матрица вида
glm::mat4 Camera::getViewMatrix() const {
    return _viewMatrix;
}

// Матрица проекции
glm::mat4 Camera::getProjectionMatrix() const {
    return _projectionMatrix;
}

// Матрица вращения
glm::mat4 Camera::getRotationMatrix() const {
    return glm::mat4_cast(_rotation);
}

// Обновление матрицы вида
void Camera::updateViewMatrix() {
    const glm::mat4 translation = glm::translate(glm::mat4(1.0f), _position);
    const glm::mat4 rotation = glm::mat4_cast(_rotation);
    _viewMatrix = glm::inverse(translation * rotation);
}

// Обновление матрицы проекции
void Camera::updateProjectionMatrix() {
    _projectionMatrix = glm::perspective(
            glm::radians(_fov),
            _screenWidth / _screenHeight,
            _nearClip,
            _farClip
    );
    _projectionMatrix[1][1] *= -1; // Коррекция для Vulkan
}

// Геттеры/сеттеры
glm::vec3 Camera::getPosition() const { return _position; }
void Camera::setPosition(const glm::vec3& position) {
    _position = position;
    updateViewMatrix();
}

glm::quat Camera::getRotation() const { return _rotation; }
void Camera::setRotation(const glm::quat& rotation) {
    _rotation = rotation;
    updateViewMatrix();
}

float Camera::getFOV() const { return _fov; }
void Camera::setFOV(float fov) {
    _fov = fov;
    updateProjectionMatrix();
}

float Camera::getScreenWidth() const { return _screenWidth; }
void Camera::setScreenWidth(float width) {
    _screenWidth = width;
    updateProjectionMatrix();
}

float Camera::getScreenHeight() const { return _screenHeight; }
void Camera::setScreenHeight(float height) {
    _screenHeight = height;
    updateProjectionMatrix();
}

// Управление углами Эйлера
glm::vec3 Camera::getEulerAngles() const {
    return glm::degrees(glm::eulerAngles(_rotation));
}

void Camera::setEulerAngles(const glm::vec3& angles) {
    _rotation = glm::quat(glm::radians(angles));
    updateViewMatrix();
}