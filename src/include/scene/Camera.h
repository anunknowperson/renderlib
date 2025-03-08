#pragma once

#include <SDL2/SDL_events.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>

class Camera {
public:
    Camera();
    Camera(const glm::vec3& position, float fov, float screenWidth, float screenHeight);

    void update(float deltaTime);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getRotationMatrix() const;

    glm::vec3 getPosition() const;
    void setPosition(const glm::vec3& position);

    glm::quat getRotation() const;
    void setRotation(const glm::quat& rotation);

    float getFOV() const;
    void setFOV(float fov);

    float getScreenWidth() const;
    void setScreenWidth(float width);

    float getScreenHeight() const;
    void setScreenHeight(float height);

    glm::vec3 getEulerAngles() const;
    void setEulerAngles(const glm::vec3& angles);

private:
    void updateViewMatrix();
    void updateProjectionMatrix();

    glm::vec3 _position {0.f};
    glm::quat _rotation {1.f, 0.f, 0.f, 0.f};
    glm::vec3 _velocity {0.f};

    float _fov {45.f};
    float _screenWidth {800.f};
    float _screenHeight {600.f};
    float _nearClip {0.1f};
    float _farClip {10000.f};

    glm::mat4 _viewMatrix {1.f};
    glm::mat4 _projectionMatrix {1.f};
};