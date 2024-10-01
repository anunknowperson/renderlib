#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera {
public:
    // Конструктор камеры
    Camera(const glm::vec3& position, float fov, float width, float height);

    // Метод для установки направления взгляда камеры на целевую точку
    void lookAt(const glm::vec3& target);

    // Метод для установки новой позиции камеры
    void setPosition(const glm::vec3& newPosition);

    // Метод для установки нового вращения камеры
    void setRotation(const glm::quat& newRotation);

    // Получение матрицы вида
    glm::mat4 getViewMatrix() const;

    // Получение текущей позиции камеры
    glm::vec3 getPosition() const;

    // Метод для изменения поля зрения камеры
    void setFOV(float newFOV);

private:
    // Метод для обновления матрицы вида на основе текущей позиции и направления
    void updateViewMatrix();

    glm::vec3 position;  // Текущая позиция камеры
    glm::vec3 direction; // Направление взгляда камеры
    glm::quat rotation;  // Вращение камеры
    float fov;           // Поле зрения камеры
    float width;         // Ширина окна просмотра
    float height;        // Высота окна просмотра
    glm::mat4 viewMatrix; // Матрица вида для рендеринга
};
