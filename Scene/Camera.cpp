#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera {
public:
    // Конструктор камеры
    Camera(const glm::vec3& position, float fov, float width, float height)
            : position(position), fov(fov), width(width), height(height) {
        // Инициализируем матрицу вида при создании камеры
        updateViewMatrix();
    }

    // Метод для установки направления взгляда камеры на целевую точку
    void lookAt(const glm::vec3& target) {
        // Вычисляем направление взгляда
        direction = glm::normalize(target - position);
        // Обновляем матрицу вида после изменения направления
        updateViewMatrix();
    }

    // Метод для установки новой позиции камеры
    void setPosition(const glm::vec3& newPosition) {
        position = newPosition;
        // Обновляем матрицу вида после изменения позиции
        updateViewMatrix();
    }

    // Метод для установки нового вращения камеры (если понадобится)
    void setRotation(const glm::quat& newRotation) {
        rotation = newRotation;
        // Можно обновить матрицу вида в зависимости от вращения
        // updateViewMatrix();
    }

    // Получение матрицы вида
    glm::mat4 getViewMatrix() const {
        return viewMatrix;
    }

    // Получение текущей позиции камеры
    glm::vec3 getPosition() const {
        return position;
    }

    // Метод для изменения поля зрения камеры
    void setFOV(float newFOV) {
        fov = newFOV;
        // Можно добавить логику для обновления проекционной матрицы
    }

private:
    // Метод для обновления матрицы вида на основе текущей позиции и направления
    void updateViewMatrix() {
        // Используем glm::lookAt для генерации матрицы вида
        viewMatrix = glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::vec3 position;  // Текущая позиция камеры
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f); // Исходное направление взгляда камеры
    glm::quat rotation;  // Вращение камеры (если потребуется)
    float fov;           // Поле зрения камеры
    float width;         // Ширина окна просмотра
    float height;        // Высота окна просмотра
    glm::mat4 viewMatrix; // Матрица вида для рендеринга
};
