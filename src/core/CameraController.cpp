#define GLM_ENABLE_EXPERIMENTAL
#include "iostream"
#include "core/CameraController.h"
#include <SDL_events.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

InputEvent SDLEventToInputEvent(const SDL_Event& event) {
    InputEvent inputEvent;

    switch (event.type) {
        case SDL_KEYDOWN:
            inputEvent.type = InputEvent::Type::KeyDown;
            inputEvent.key = event.key.keysym.sym;
            break;

        case SDL_KEYUP:
            inputEvent.type = InputEvent::Type::KeyUp;
            inputEvent.key = event.key.keysym.sym;
            break;

        case SDL_MOUSEMOTION:
            inputEvent.type = InputEvent::Type::MouseMove;
            inputEvent.x = event.motion.x;
            inputEvent.y = event.motion.y;
            inputEvent.deltaX = event.motion.xrel;
            inputEvent.deltaY = event.motion.yrel;
            break;

        case SDL_MOUSEBUTTONDOWN:
            inputEvent.type = InputEvent::Type::MouseDown;
            inputEvent.key = event.button.button;
            inputEvent.x = event.button.x;
            inputEvent.y = event.button.y;
            break;

        case SDL_MOUSEBUTTONUP:
            inputEvent.type = InputEvent::Type::MouseUp;
            inputEvent.key = event.button.button;
            inputEvent.x = event.button.x;
            inputEvent.y = event.button.y;
            break;

        case SDL_MOUSEWHEEL:
            inputEvent.type = InputEvent::Type::MouseWheel;
            inputEvent.deltaX = event.wheel.x;
            inputEvent.deltaY = event.wheel.y;
            break;

        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                inputEvent.type = InputEvent::Type::WindowResize;
                inputEvent.x = event.window.data1;
                inputEvent.y = event.window.data2;
            }
            break;
    }

    return inputEvent;
}

bool ICameraController::processSDLEvent(const SDL_Event& event) {
    return handleInput(SDLEventToInputEvent(event));
}

BaseCameraController::BaseCameraController(std::shared_ptr<ICamera> camera)
    : _camera(std::move(camera)) {}

void BaseCameraController::setMovementSpeed(float speed) {
    _movementSpeed = speed;
}

float BaseCameraController::getMovementSpeed() const {
    return _movementSpeed;
}

void BaseCameraController::setRotationSpeed(float speed) {
    _rotationSpeed = speed;
}

float BaseCameraController::getRotationSpeed() const {
    return _rotationSpeed;
}

FPSCameraController::FPSCameraController(std::shared_ptr<ICamera> camera)
    : BaseCameraController(std::move(camera)) {
    glm::vec3 angles = _camera->getEulerAngles();
    _pitch = angles.x;
    _yaw = angles.y;
}

void FPSCameraController::update(float deltaTime) {
    float moveAmount = _movementSpeed * deltaTime;
    glm::vec3 position = _camera->getPosition();
    glm::quat rotation = _camera->getRotation();

    glm::vec3 forward = rotation * glm::vec3(0, 0, -1);
    glm::vec3 right = rotation * glm::vec3(1, 0, 0);
    glm::vec3 up = rotation * glm::vec3(0, 1, 0);

    if (_movingForward)
        position += forward * moveAmount;
    if (_movingBackward)
        position -= forward * moveAmount;
    if (_movingRight)
        position += right * moveAmount;
    if (_movingLeft)
        position -= right * moveAmount;
    if (_movingUp)
        position += up * moveAmount;
    if (_movingDown)
        position -= up * moveAmount;

    _camera->setPosition(position);
}

bool FPSCameraController::handleInput(const InputEvent& event) {
    switch (event.type) {
        case InputEvent::Type::KeyDown:
            if (event.key == SDLK_w || event.key == SDLK_UP)
                _movingForward = true;
            else if (event.key == SDLK_s || event.key == SDLK_DOWN)
                _movingBackward = true;
            else if (event.key == SDLK_a || event.key == SDLK_LEFT)
                _movingLeft = true;
            else if (event.key == SDLK_d || event.key == SDLK_RIGHT)
                _movingRight = true;
            else if (event.key == SDLK_SPACE)
                _movingUp = true;
            else if (event.key == SDLK_LSHIFT)
                _movingDown = true;
            else
                return false;
            return true;

        case InputEvent::Type::KeyUp:
            if (event.key == SDLK_w || event.key == SDLK_UP)
                _movingForward = false;
            else if (event.key == SDLK_s || event.key == SDLK_DOWN)
                _movingBackward = false;
            else if (event.key == SDLK_a || event.key == SDLK_LEFT)
                _movingLeft = false;
            else if (event.key == SDLK_d || event.key == SDLK_RIGHT)
                _movingRight = false;
            else if (event.key == SDLK_SPACE)
                _movingUp = false;
            else if (event.key == SDLK_LSHIFT)
                _movingDown = false;
            else
                return false;
            return true;

        case InputEvent::Type::MouseDown:
            if (event.key == SDL_BUTTON_RIGHT) {
                _rotating = true;
                _lastMouseX = static_cast<float>(event.x);
                _lastMouseY = static_cast<float>(event.y);
                SDL_SetRelativeMouseMode(SDL_TRUE);
                return true;
            }
        return false;

        case InputEvent::Type::MouseUp:
            if (event.key == SDL_BUTTON_RIGHT) {
                _rotating = false;
                SDL_SetRelativeMouseMode(SDL_FALSE);
                return true;
            }
        return false;

        case InputEvent::Type::MouseMove:
            if (_rotating) {
                float deltaX = static_cast<float>(event.x) - _lastMouseX;
                float deltaY = static_cast<float>(event.y) - _lastMouseY;

                _lastMouseX = static_cast<float>(event.x);
                _lastMouseY = static_cast<float>(event.y);

                _yaw -= deltaX * _rotationSpeed;
                _pitch -= deltaY * _rotationSpeed;

                _pitch = std::max(-89.0f, std::min(89.0f, _pitch));

                glm::quat rotation = glm::quat(glm::vec3(
                    glm::radians(_pitch),
                    glm::radians(_yaw),
                    0.0f
                ));

                _camera->setRotation(rotation);
                return true;
            }
            return false;

        case InputEvent::Type::WindowResize:
            _camera->updateViewport(static_cast<float>(event.x), static_cast<float>(event.y));
            return true;

        default:
            return false;
    }
}

OrbitCameraController::OrbitCameraController(std::shared_ptr<ICamera> camera)
    : BaseCameraController(std::move(camera)) {
    _distance = glm::length(_camera->getPosition() - _target);

    glm::vec3 direction = glm::normalize(_target - _camera->getPosition());
    _pitch = glm::degrees(asin(direction.y));
    _yaw = glm::degrees(atan2(direction.x, direction.z));
}

void OrbitCameraController::update(float /*deltaTime*/) {
    float radPitch = glm::radians(_pitch);
    float radYaw = glm::radians(_yaw);

    float cosP = cos(radPitch);
    glm::vec3 position = _target + glm::vec3(
        cosP * sin(radYaw),
        sin(radPitch),
        cosP * cos(radYaw)
    ) * _distance;

    _camera->setPosition(position);

    glm::vec3 direction = glm::normalize(_target - position);
    glm::vec3 worldUp = glm::vec3(0, 1, 0);
    glm::vec3 right = glm::normalize(glm::cross(direction, worldUp));
    glm::vec3 up = glm::normalize(glm::cross(right, direction));

    glm::mat3 rotMatrix(right, up, -direction);
    _camera->setRotation(glm::quat_cast(rotMatrix));
}

bool OrbitCameraController::handleInput(const InputEvent& event) {
    switch (event.type) {
        case InputEvent::Type::MouseDown:
            if (event.key == SDL_BUTTON_RIGHT) {
                _rotating = true;
                _lastMouseX = static_cast<float>(event.x);
                _lastMouseY = static_cast<float>(event.y);
                return true;
            }
            else if (event.key == SDL_BUTTON_MIDDLE) {
                _panning = true;
                _lastMouseX = static_cast<float>(event.x);
                _lastMouseY = static_cast<float>(event.y);
                return true;
            }
            return false;

        case InputEvent::Type::MouseUp:
            if (event.key == SDL_BUTTON_RIGHT) {
                _rotating = false;
                return true;
            }
            else if (event.key == SDL_BUTTON_MIDDLE) {
                _panning = false;
                return true;
            }
            return false;

        case InputEvent::Type::MouseMove:
            if (_rotating) {
                float deltaX = static_cast<float>(event.x) - _lastMouseX;
                float deltaY = static_cast<float>(event.y) - _lastMouseY;

                _lastMouseX = static_cast<float>(event.x);
                _lastMouseY = static_cast<float>(event.y);

                _yaw += deltaX * _rotationSpeed;
                _pitch -= deltaY * _rotationSpeed;

                _pitch = std::max(-89.0f, std::min(89.0f, _pitch));

                return true;
            }
            else if (_panning) {
                float deltaX = static_cast<float>(event.x) - _lastMouseX;
                float deltaY = static_cast<float>(event.y) - _lastMouseY;

                _lastMouseX = static_cast<float>(event.x);
                _lastMouseY = static_cast<float>(event.y);

                glm::vec3 right = _camera->getRotation() * glm::vec3(1, 0, 0);
                glm::vec3 up = _camera->getRotation() * glm::vec3(0, 1, 0);

                _target -= right * (deltaX * 0.01f * _distance);
                _target += up * (deltaY * 0.01f * _distance);

                return true;
            }
            return false;

        case InputEvent::Type::MouseWheel:
            // Zoom in/out
            _distance -= event.deltaY * (_distance * 0.1f);
            _distance = std::max(0.1f, _distance);
            return true;

        case InputEvent::Type::WindowResize:
            _camera->updateViewport(static_cast<float>(event.x), static_cast<float>(event.y));
            return true;

        default:
            return false;
    }
}

void OrbitCameraController::setTarget(const glm::vec3& target) {
    _target = target;
}

CameraController::CameraController(Camera& camera)
    : FPSCameraController(std::shared_ptr<ICamera>(&camera, [](ICamera*){/* non-owning */})) {
}