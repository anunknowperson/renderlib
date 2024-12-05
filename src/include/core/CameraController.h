#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <SDL.h>
#include "scene/Camera.h"

class CameraController {
public:
    explicit CameraController(Camera& camera);

    void setPosition(const glm::vec3& newPosition);
    glm::vec3 getPosition() const;

    void setRotation(const glm::quat& newRotation);
    glm::quat getRotation() const;

    void setPitch(float newPitch);
    void setYaw(float newYaw);
    float getPitch() const;
    float getYaw() const;

    void lookAt(const glm::vec3& target);
    void setFOV(float newFOV);
    float getFOV() const;

    void setScreenWidth(float screenWidth);
    float getScreenWidth() const;

    void setScreenHeight(float screenHeight);
    float getScreenHeight() const;

    glm::mat4 getViewMatrix() const; // Method to get the view matrix
    void updateViewMatrix(); // Method to update the view matrix

    void processSDLEvent(const SDL_Event& event); // Method to process SDL events

    void update(); // Method to update the camera

private:
    Camera& _camera; // Reference to the camera
    float _pitch; // Pitch angle
    float _yaw; // Yaw angle
};