#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera {
public:
    Camera();
    Camera(const glm::vec3& position, float fov, float width, float height);

    glm::vec3 getPosition() const;
    glm::quat getRotation() const;
    float getFOV() const;

    // Публичные данные, с которыми будет работать контроллер
    glm::vec3 position;
    glm::quat rotation;
    float fov;
    float width;
    float height;

private:
    glm::vec3 direction;
    glm::mat4 viewMatrix;
};