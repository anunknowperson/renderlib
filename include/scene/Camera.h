#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// Класс Camera отвечает только за хранение данных о камере
// Имплементация содержится в CameraController
class Camera {
public:
    Camera();
    Camera(const glm::vec3& position, float fov, float width, float height);

    glm::vec3 getPosition() const;
    glm::quat getRotation() const;
    float getFOV() const;

    // Публичные данные, которые контроллер будет использовать для управления
    glm::vec3 position;
    glm::quat rotation;
    float fov;
    // Разрешение экрана
    float width;
    float height;

private:
    glm::vec3 direction;
    glm::mat4 viewMatrix;
};