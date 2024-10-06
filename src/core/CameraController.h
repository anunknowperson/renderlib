#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <SDL.h>
#include "scene/Camera.h"

class CameraController {
public:
    CameraController(Camera& camera);

    void setPosition(const glm::vec3& newPosition);
    glm::vec3 getPosition() const;

    void setRotation(const glm::quat& newRotation);
    glm::quat getRotation() const;

    void setPitch(float newPitch);
    void setYaw(float newYaw);
    float getPitch() const;
    float getYaw() const;

    void lookAt(const glm::vec3& target);
    void setFOV(float newFOV);
    float getFOV() const;

    glm::mat4 getViewMatrix() const; // Метод для получения матрицы вида
    void updateViewMatrix(); // Метод для обновления матрицы вида

    void processSDLEvent(const SDL_Event& event); // Метод для обработки событий SDL

private:
    Camera& camera; // Ссылка на камеру
    float pitch; // Угол наклона
    float yaw; // Угол поворота
};
