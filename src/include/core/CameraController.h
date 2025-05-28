#pragma once

#include "SDL2/SDL.h"
#include <functional>
#include <memory>
#include <unordered_map>

#include "scene/Camera.h"

// Abstract input event that can be adapted for different input systems
struct InputEvent {
    enum class Type {
        KeyDown,
        KeyUp,
        MouseMove,
        MouseDown,
        MouseUp,
        MouseWheel,
        WindowResize
    };

    Type type;
    int key = 0;
    int x = 0;
    int y = 0;
    int deltaX = 0;
    int deltaY = 0;
};

InputEvent SDLEventToInputEvent(const SDL_Event& event);

class ICameraController {
public:
    virtual ~ICameraController() = default;
    virtual void update(float deltaTime = 1.0f/60.0f) = 0;
    virtual bool handleInput(const InputEvent& event) = 0;

    bool processSDLEvent(const SDL_Event& event);
};

// Base controller with common functionality
class BaseCameraController : public ICameraController {
public:
    explicit BaseCameraController(std::shared_ptr<ICamera> camera);

    void setMovementSpeed(float speed);
    float getMovementSpeed() const;
    void setRotationSpeed(float speed);
    float getRotationSpeed() const;

protected:
    std::shared_ptr<ICamera> _camera;
    float _movementSpeed = 5.0f;
    float _rotationSpeed = 0.3f;
};

// First-person style controller
class FPSCameraController : public BaseCameraController {
public:
    explicit FPSCameraController(std::shared_ptr<ICamera> camera);

    void update(float deltaTime = 1.0f/60.0f) override;
    bool handleInput(const InputEvent& event) override;

private:
    bool _movingForward = false;
    bool _movingBackward = false;
    bool _movingLeft = false;
    bool _movingRight = false;
    bool _movingUp = false;
    bool _movingDown = false;

    bool _rotating = false;
    float _pitch = 0.0f;
    float _yaw = 0.0f;
};

// Orbit style controller
class OrbitCameraController : public BaseCameraController {
public:
    explicit OrbitCameraController(std::shared_ptr<ICamera> camera);

    void update(float deltaTime = 1.0f/60.0f) override;
    bool handleInput(const InputEvent& event) override;
    void setTarget(const glm::vec3& target);

private:
    float _panSpeed = 0.01f;
    float _zoomFactor = 0.1f;
    glm::vec3 _target = glm::vec3(0.0f);
    float _distance = 10.0f;
    float _pitch = 0.0f;
    float _yaw = 0.0f;

    bool _rotating = false;
    bool _panning = false;
    float _lastMouseX = 0.0f;
    float _lastMouseY = 0.0f;
};

// For backward compatibility
class CameraController : public FPSCameraController {
public:
    explicit CameraController(Camera& camera);
};