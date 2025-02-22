#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <SDL2/SDL_events.h>

class Camera {
public:
    Camera();
    Camera(const glm::vec3& position, float fov, float screenWidth, float screenHeight);

    // Методы трансформации
    void update();
    void processSDLEvent(const SDL_Event& e);

    // Методы доступа
    glm::vec3 getPosition() const;
    void setPosition(const glm::vec3& position);

    glm::quat getRotation() const;
    void setRotation(const glm::quat& rotation);

    [[nodiscard]] glm::mat4 getViewMatrix() const;
    [[nodiscard]] glm::mat4 getRotationMatrix() const;

    float getFOV() const;
    void setFOV(float fov);

    float getScreenWidth() const;
    void setScreenWidth(float screenWidth);

    float getScreenHeight() const;
    void setScreenHeight(float screenHeight);

    float getPitch() const;
    void setPitch(float pitch);
    void rotatePitch(float angle);

private:
    void updateViewMatrix();

    // Состояние камеры
    glm::vec3 _position;
    glm::quat _rotation;
    glm::vec3 _velocity{0.f};
    float _yaw = 0.f;
    float _pitch = 0.f;

    // Параметры проекции
    float _fov;
    float _screenWidth;
    float _screenHeight;

    // Кэшированные матрицы
    glm::mat4 _viewMatrix;
};