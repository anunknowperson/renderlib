#define GLM_ENABLE_EXPERIMENTAL
#include "scene/Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Camera::Camera()
    : _position(glm::vec3(0.0f, 0.0f, 3.0f)),
      _rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
      _fov(45.0f),
      _screenWidth(800.0f),
      _screenHeight(600.0f),
      _velocity(glm::vec3(0.0f)),
      _yaw(0.0f),
      _pitch(0.0f) {
    updateViewMatrix();
}

Camera::Camera(const glm::vec3& position, float fov, float screenWidth, float screenHeight)
    : _position(position),
      _rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
      _fov(fov),
      _screenWidth(screenWidth),
      _screenHeight(screenHeight),
      _velocity(glm::vec3(0.0f)),
      _yaw(0.0f),
      _pitch(0.0f) {
    updateViewMatrix();
}

void Camera::update() {
    const glm::mat4 cameraRotation = getRotationMatrix();
    _position += glm::vec3(cameraRotation * glm::vec4(_velocity * 0.05f, 0.0f));
    updateViewMatrix();
}

void Camera::processSDLEvent(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        switch(e.key.keysym.sym) {
            case SDLK_w: _velocity.z = -1.0f; break;
            case SDLK_s: _velocity.z =  1.0f; break;
            case SDLK_a: _velocity.x = -1.0f; break;
            case SDLK_d: _velocity.x =  1.0f; break;
        }
    }

    if (e.type == SDL_KEYUP) {
        switch(e.key.keysym.sym) {
            case SDLK_w:
            case SDLK_s: _velocity.z = 0.0f; break;
            case SDLK_a:
            case SDLK_d: _velocity.x = 0.0f; break;
        }
    }

    if (e.type == SDL_MOUSEMOTION) {
        _yaw   += static_cast<float>(e.motion.xrel) / 200.0f;
        _pitch -= static_cast<float>(e.motion.yrel) / 200.0f;

        // Ограничение угла pitch
        _pitch = glm::clamp(_pitch, -glm::half_pi<float>() + 0.1f,
                            glm::half_pi<float>() - 0.1f);
    }
}

glm::mat4 Camera::getRotationMatrix() const {
    const glm::quat pitchRot = glm::angleAxis(_pitch, glm::vec3{1.0f, 0.0f, 0.0f});
    const glm::quat yawRot = glm::angleAxis(_yaw, glm::vec3{0.0f, 1.0f, 0.0f});
    return glm::mat4_cast(yawRot * pitchRot);
}

// Остальные методы
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
void Camera::setFOV(float fov) { _fov = fov; }

float Camera::getScreenWidth() const { return _screenWidth; }
void Camera::setScreenWidth(float width) { _screenWidth = width; }

float Camera::getScreenHeight() const { return _screenHeight; }
void Camera::setScreenHeight(float height) { _screenHeight = height; }

float Camera::getPitch() const { return glm::degrees(_pitch); }

void Camera::setPitch(float pitch) {
    _pitch = glm::radians(pitch);
    updateViewMatrix();
}

void Camera::rotatePitch(float angle) {
    _pitch += glm::radians(angle);
    updateViewMatrix();
}

glm::mat4 Camera::getViewMatrix() const {
    return _viewMatrix;
}

void Camera::updateViewMatrix() {
    const glm::vec3 forward = glm::normalize(glm::vec3(
            glm::sin(_yaw) * glm::cos(_pitch),
            glm::sin(_pitch),
            glm::cos(_yaw) * glm::cos(_pitch)
                    ));

    _viewMatrix = glm::lookAt(
            _position,
            _position + forward,
            glm::vec3(0.0f, 1.0f, 0.0f)
    );
}