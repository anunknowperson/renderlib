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
    glm::vec3 getPosition() const;

    void setRotation(const glm::quat& newRotation);
    glm::quat getRotation() const;

    glm::mat4 getViewMatrix() const;

    void setFOV(float newFOV);
    float getFOV() const;

    glm::vec3 getDirection() const;

    void update();
    void processSDLEvent(const SDL_Event& event);

    void setVelocity(const glm::vec3& newVelocity);
    glm::vec3 getVelocity() const;

    void setPitch(float newPitch); // Декларация метода
    float getPitch() const;        // Декларация метода

    void setYaw(float newYaw);     // Декларация метода
    float getYaw() const;          // Декларация метода

    glm::vec3 velocity;
    glm::vec3 position;

// Угол наклона и поворота
    float pitch;
    float yaw;
private:
    glm::vec3 direction;
    glm::quat rotation;
    float fov;
    float width;
    float height;
    glm::mat4 viewMatrix;

    void updateViewMatrix();
};
