#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

class ICamera {
public:
    virtual ~ICamera() = default;
    virtual glm::mat4 getViewMatrix() const = 0;
    virtual glm::mat4 getProjectionMatrix() const = 0;
    virtual void updateViewport(float width, float height) = 0;
    virtual glm::vec3 getPosition() const = 0;
    virtual void setPosition(const glm::vec3& position) = 0;
    virtual glm::quat getRotation() const = 0;
    virtual void setRotation(const glm::quat& rotation) = 0;
    virtual glm::vec3 getEulerAngles() const = 0;
};

// Base implementation with common functionality
class BaseCamera : public ICamera {
public:
    BaseCamera(const glm::vec3& position = glm::vec3(0.0f),
               float nearClip = 0.1f,
               float farClip = 1000.0f);

    glm::mat4 getViewMatrix() const override;
    glm::mat4 getProjectionMatrix() const override;
    glm::vec3 getPosition() const override;
    void setPosition(const glm::vec3& position) override;
    glm::quat getRotation() const override;
    void setRotation(const glm::quat& rotation) override;
    glm::vec3 getEulerAngles() const;
    void setEulerAngles(const glm::vec3& angles);

protected:
    glm::vec3 _position{0.0f};
    glm::quat _rotation{1.0f, 0.0f, 0.0f, 0.0f};
    float _nearClip;
    float _farClip;
    mutable glm::mat4 _viewMatrix{1.0f};
    mutable glm::mat4 _projectionMatrix{1.0f};
    mutable bool _viewDirty = true;
    mutable bool _projDirty = true;

    virtual void updateViewMatrix() const;
    virtual void updateProjectionMatrix() const = 0;
};

// Perspective camera with FOV control
class PerspectiveCamera : public BaseCamera {
public:
    PerspectiveCamera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 5.0f),
                      float fov = 70.0f,
                      float aspectRatio = 16.0f/9.0f,
                      float nearClip = 0.1f,
                      float farClip = 1000.0f);

    void updateViewport(float width, float height) override;
    float getFOV() const;
    void setFOV(float fov);

protected:
    float _fov;
    float _aspectRatio;

    void updateProjectionMatrix() const override;
};

// Orthographic camera
class OrthographicCamera : public BaseCamera {
public:
    OrthographicCamera(const glm::vec3& position = glm::vec3(0.0f),
                       float size = 10.0f,
                       float aspectRatio = 16.0f/9.0f,
                       float nearClip = 0.1f,
                       float farClip = 1000.0f);

    void updateViewport(float width, float height) override;
    float getSize() const;
    void setSize(float size);

protected:
    float _size;
    float _aspectRatio;

    void updateProjectionMatrix() const override;
};

class Camera : public PerspectiveCamera {
public:
    Camera();
    Camera(const glm::vec3& position,
           float fov,
           float screenWidth,
           float screenHeight,
           float nearClip,
           float farClip);

    float getScreenWidth() const;
    void setScreenWidth(float width);
    float getScreenHeight() const;
    void setScreenHeight(float height);
};