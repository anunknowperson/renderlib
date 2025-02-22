#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <SDL2/SDL_events.h>

class Camera {
public:
    Camera();
    Camera(const glm::vec3& position, float fov, float screenWidth, float screenHeight);

    // Обновление состояния камеры
    void update(float deltaTime);
    void processSDLEvent(const SDL_Event& e);

    // Геттеры/сеттеры
    glm::vec3 getPosition() const;
    void setPosition(const glm::vec3& position);

    glm::quat getRotation() const;
    void setRotation(const glm::quat& rotation);

    [[nodiscard]] glm::mat4 getViewMatrix() const;
    [[nodiscard]] glm::mat4 getProjectionMatrix() const;
    [[nodiscard]] glm::mat4 getRotationMatrix() const;

    float getFOV() const;
    void setFOV(float fov);

    float getScreenWidth() const;
    void setScreenWidth(float screenWidth);

    float getScreenHeight() const;
    void setScreenHeight(float screenHeight);

    // Управление углами Эйлера
    glm::vec3 getEulerAngles() const;
    void setEulerAngles(const glm::vec3& angles);

private:
    void updateViewMatrix();
    void updateProjectionMatrix();

    // Состояние камеры
    glm::vec3 _position {0.f};
    glm::quat _rotation {1.f, 0.f, 0.f, 0.f};
    glm::vec3 _velocity {0.f};

    // Параметры проекции
    float _fov {45.f};
    float _screenWidth {800.f};
    float _screenHeight {600.f};
    float _nearClip {0.1f};
    float _farClip {10000.f};

    // Кэшированные матрицы
    glm::mat4 _viewMatrix {1.f};
    glm::mat4 _projectionMatrix {1.f};
};