#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "scene/Camera.h"

// Класс CameraController отвечает за управление камерой, такие как
// позиция, ориентация, угол наклона и поворота, а также обновление матрицы
// вида.
class CameraController {
public:
    // Конструктор, принимающий ссылку на объект камеры.
    // Позволяет управлять камерой через данный контроллер.
    explicit CameraController(Camera& camera);

    // Устанавливает новую позицию камеры.
    void setPosition(const glm::vec3& newPosition);

    // Возвращает текущую позицию камеры.
    glm::vec3 getPosition() const;

    // Устанавливает новое значение ориентации камеры.
    // newRotation - новый кватернион, описывающий ориентацию камеры.
    void setRotation(const glm::quat& newRotation);

    // Возвращает текущую ориентацию камеры.
    glm::quat getRotation() const;

    // Устанавливает угол наклона камеры (pitch).
    void setPitch(float newPitch);

    // Устанавливает угол поворота камеры (yaw).
    void setYaw(float newYaw);

    // Возвращает текущий угол наклона камеры (pitch).
    float getPitch() const;

    // Возвращает текущий угол поворота камеры (yaw).
    float getYaw() const;

    // Устанавливает ориентацию камеры так, чтобы она смотрела на заданную
    // точку. target - точка, на которую должна смотреть камера.
    void lookAt(const glm::vec3& target);

    // Устанавливает новое значение угла обзора (FOV) камеры.
    void setFOV(float newFOV);

    // Возвращает текущий угол обзора (FOV) камеры.
    float getFOV() const;

    // Возвращает матрицу вида камеры
    glm::mat4 getViewMatrix() const;

    // Обновляет матрицу вида камеры на основе текущей позиции и ориентации.
    void updateViewMatrix();

    // Метод для обработки событий SDL, таких как движения мыши и нажатия
    // клавиш. event - событие, которое нужно обработать.
    void processSDLEvent(const SDL_Event& event);

private:
    Camera& _camera;  // Ссылка на объект камеры, управляемой этим контроллером
    float _pitch;     // Угол наклона камеры (по оси X)
    float _yaw;       // Угол поворота камеры (по оси Y)
};
