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

    void setPosition(const glm::vec3& newPosition);  // Устанавливает новую
                                                     // позицию камеры
    glm::vec3 getPosition() const;  // Возвращает текущую позицию камеры

    void setRotation(
            const glm::quat& newRotation);  // Устанавливает ориентацию камеры
    glm::quat getRotation() const;  // Возвращает текущую ориентацию камеры

    void setPitch(float newPitch);  // Устанавливает угол наклона камеры (pitch)
    float getPitch() const;  // Возвращает текущий угол наклона

    void setYaw(float newYaw);  // Устанавливает угол поворота камеры (yaw)
    float getYaw() const;  // Возвращает текущий угол поворота

    void lookAt(const glm::vec3& target);  // Устанавливает ориентацию камеры,
                                           // чтобы она смотрела на target

    void setFOV(float newFOV);  // Устанавливает угол обзора
    float getFOV() const;  // Возвращает текущий угол обзора

    // Обрабатывает события SDL, такие как нажатия клавиш (WASD) для перемещения
    // камеры и движения мыши для изменения ориентации камеры.
    // event - событие, которое нужно обработать.
    void processSDLEvent(const SDL_Event& event);

private:
    Camera& _camera;  // Ссылка на объект камеры, управляемой этим контроллером
    float _pitch;     // Угол наклона камеры (по оси X)
    float _yaw;       // Угол поворота камеры (по оси Y)
};
