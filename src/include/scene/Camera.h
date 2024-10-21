#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// Класс Camera отвечает только за хранение данных о камере
// Имплементация содержится в CameraController
class Camera {
public:
    // Конструктор по умолчанию.
    // Устанавливает позицию камеры в начало координат, угол обзора 45 градусов,
    // а также разрешение экрана 800x600

    Camera();
    // Конструктор с параметрами.
    // Позволяет задать положение камеры, угол обзора (в градусах) и размеры
    // экрана. Параметры:
    // - position: начальная позиция камеры
    // - fov: поле зрения
    // - width: ширина экрана
    // - height: высота экрана
    Camera(const glm::vec3& position, float fov, float width, float height);

    // Возвращает текущее положение камеры
    glm::vec3 getPosition() const;

    // Возвращает текущую ориентацию камеры в виде кватерниона
    glm::quat getRotation() const;

    // Возвращает текущее значение угла обзора)
    float getFOV() const;

    // Публичные поля, которые контроллер будет использовать для управления
    // камерой
    glm::vec3 position;
    glm::quat rotation;
    float fov;     // Угол обзора в градусах
    float width;   // Ширина экрана
    float height;  // Высота экрана

private:
    glm::vec3 _direction;  // Направление взгляда камеры
    glm::mat4 _viewMatrix;  // Матрица вида, используемая для преобразований
};