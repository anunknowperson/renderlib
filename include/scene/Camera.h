#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <SDL.h> // Подключение SDL для обработки событий

class Camera {
public:
    Camera();
    Camera(const glm::vec3& position, float fov, float width, float height);

    void lookAt(const glm::vec3& target);
    void setPosition(const glm::vec3& newPosition);
    void setRotation(const glm::quat& newRotation);
    glm::mat4 getViewMatrix() const;
    glm::vec3 getPosition() const;
    void setFOV(float newFOV);
    void update();
    void processSDLEvent(const SDL_Event& event);

private:
    void updateViewMatrix();

    glm::vec3 position;
    glm::vec3 direction;
    glm::quat rotation;
    float fov;
    float width;
    float height;
    glm::mat4 viewMatrix;
};