#define GLM_ENABLE_EXPERIMENTAL
#include <gtest/gtest.h>
#include <SDL2/SDL.h>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "core/CameraController.h"
#include "scene/Camera.h"

class TestCamera : public ICamera {
public:
    glm::vec3 position{0.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f}; // Identity quaternion
    float viewportWidth{800.0f};
    float viewportHeight{600.0f};

    void setPosition(const glm::vec3& pos) override { position = pos; }
    glm::vec3 getPosition() const override { return position; }
    void setRotation(const glm::quat& rot) override { rotation = rot; }
    glm::quat getRotation() const override { return rotation; }
    void updateViewport(float width, float height) override {
        viewportWidth = width;
        viewportHeight = height;
    }
    glm::mat4 getViewMatrix() const override { return glm::mat4(1.0f); }
    glm::mat4 getProjectionMatrix() const override { return glm::mat4(1.0f); }
    glm::vec3 getEulerAngles() const override {
        return glm::degrees(glm::eulerAngles(rotation));
    }
};

// Test FPS camera movement
TEST(CameraControllerTest, FPSControllerMovement) {
    auto camera = std::make_shared<TestCamera>();
    FPSCameraController controller(camera);
    controller.setMovementSpeed(1.0f);

    // Press forward key
    InputEvent keyDown{InputEvent::Type::KeyDown, SDLK_w};
    EXPECT_TRUE(controller.handleInput(keyDown));

    // Update with 1 second delta
    controller.update(1.0f);

    // Check if moved in -Z direction (forward)
    EXPECT_LT(camera->position.z, 0.0f);

    // Test multiple keys
    InputEvent rightDown{InputEvent::Type::KeyDown, SDLK_d};
    controller.handleInput(rightDown);
    controller.update(1.0f);

    // Should have moved right too
    EXPECT_GT(camera->position.x, 0.0f);
}

// Test FPS camera rotation
TEST(CameraControllerTest, FPSControllerRotation) {
    auto camera = std::make_shared<TestCamera>();
    FPSCameraController controller(camera);
    controller.setRotationSpeed(0.1f);

    // Start rotation (right mouse button)
    InputEvent mouseDown{InputEvent::Type::MouseDown, SDL_BUTTON_RIGHT, 100, 100};
    controller.handleInput(mouseDown);

    // Simulate mouse movement
    InputEvent mouseMove{InputEvent::Type::MouseMove, 0, 120, 100};
    mouseMove.deltaX = 20; // Move right
    mouseMove.deltaY = 0;
    controller.handleInput(mouseMove);

    // Yaw should have changed
    glm::vec3 angles = camera->getEulerAngles();
    EXPECT_NE(angles.y, 0.0f);

    // Test pitch limits
    mouseMove.deltaX = 0;
    mouseMove.deltaY = -900; // Extreme upward movement
    controller.handleInput(mouseMove);

    angles = camera->getEulerAngles();
    EXPECT_LE(angles.x, 89.0f); // Should be clamped
}

// Test orbit camera functionality
TEST(CameraControllerTest, OrbitControllerFunctionality) {
    auto camera = std::make_shared<TestCamera>();
    camera->position = glm::vec3(0.0f, 0.0f, 5.0f);

    OrbitCameraController controller(camera);
    controller.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));

    // Test zoom with mouse wheel
    float initialDistance = glm::length(camera->position);

    InputEvent zoomIn{InputEvent::Type::MouseWheel};
    zoomIn.deltaY = 1.0f; // Zoom in
    controller.handleInput(zoomIn);
    controller.update(0.1f);

    float newDistance = glm::length(camera->position);
    EXPECT_LT(newDistance, initialDistance);

    // Test panning
    InputEvent panStart{InputEvent::Type::MouseDown, SDL_BUTTON_MIDDLE, 100, 100};
    controller.handleInput(panStart);

    InputEvent panMove{InputEvent::Type::MouseMove, 0, 120, 100};
    panMove.deltaX = 20;
    controller.handleInput(panMove);
    controller.update(0.1f);

    // Position should have changed but distance should remain similar
    EXPECT_NE(camera->position.x, 0.0f);
    EXPECT_NEAR(glm::length(camera->position), newDistance, 0.1f);
}

// Test window resize handling
TEST(CameraControllerTest, WindowResizeHandling) {
    auto camera = std::make_shared<TestCamera>();
    FPSCameraController controller(camera);

    InputEvent resizeEvent{InputEvent::Type::WindowResize};
    resizeEvent.x = 1920;
    resizeEvent.y = 1080;

    EXPECT_TRUE(controller.handleInput(resizeEvent));
    EXPECT_EQ(camera->viewportWidth, 1920.0f);
    EXPECT_EQ(camera->viewportHeight, 1080.0f);
}

// Test unhandled events
TEST(CameraControllerTest, UnhandledEvents) {
    auto camera = std::make_shared<TestCamera>();
    FPSCameraController controller(camera);

    // Key that isn't mapped
    InputEvent unusedKey{InputEvent::Type::KeyDown, SDLK_t};
    EXPECT_FALSE(controller.handleInput(unusedKey));

    // Left mouse button (not used by FPS controller)
    InputEvent leftMouse{InputEvent::Type::MouseDown, SDL_BUTTON_LEFT};
    EXPECT_FALSE(controller.handleInput(leftMouse));
}

// Test diagonal movement (multiple keys pressed)
TEST(CameraControllerTest, DiagonalMovement) {
    auto camera = std::make_shared<TestCamera>();
    FPSCameraController controller(camera);
    controller.setMovementSpeed(1.0f);

    // Press forward+right keys simultaneously
    EXPECT_TRUE(controller.handleInput({InputEvent::Type::KeyDown, SDLK_w}));
    EXPECT_TRUE(controller.handleInput({InputEvent::Type::KeyDown, SDLK_d}));

    controller.update(1.0f);

    // Should have moved forward and right
    EXPECT_LT(camera->position.z, 0.0f);
    EXPECT_GT(camera->position.x, 0.0f);

    // Release one key, continue moving in other direction
    EXPECT_TRUE(controller.handleInput({InputEvent::Type::KeyUp, SDLK_w}));
    controller.update(1.0f);

    // X should continue increasing, Z should stop changing
    float prevZ = camera->position.z;
    float prevX = camera->position.x;
    controller.update(1.0f);
    EXPECT_GT(camera->position.x, prevX);
    EXPECT_FLOAT_EQ(camera->position.z, prevZ);
}

// Test SDL event processing
TEST(CameraControllerTest, SDLEventProcessing) {
    auto camera = std::make_shared<TestCamera>();
    FPSCameraController controller(camera);

    // Simulate SDL keydown event
    SDL_Event sdlEvent;
    sdlEvent.type = SDL_KEYDOWN;
    sdlEvent.key.keysym.sym = SDLK_w;

    EXPECT_TRUE(controller.processSDLEvent(sdlEvent));

    // Simulate SDL window resize event
    sdlEvent.type = SDL_WINDOWEVENT;
    sdlEvent.window.event = SDL_WINDOWEVENT_RESIZED;
    sdlEvent.window.data1 = 1024;
    sdlEvent.window.data2 = 768;

    EXPECT_TRUE(controller.processSDLEvent(sdlEvent));
    EXPECT_EQ(camera->viewportWidth, 1024.0f);
    EXPECT_EQ(camera->viewportHeight, 768.0f);

    // Simulate unhandled SDL event
    sdlEvent.type = SDL_WINDOWEVENT;
    sdlEvent.window.event = SDL_WINDOWEVENT_FOCUS_LOST;
    EXPECT_FALSE(controller.processSDLEvent(sdlEvent));
}

// Test orbit controller target setting
TEST(CameraControllerTest, OrbitControllerTargetChange) {
    auto camera = std::make_shared<TestCamera>();
    camera->position = glm::vec3(0.0f, 0.0f, 5.0f);

    OrbitCameraController controller(camera);
    controller.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    controller.update(0.1f);

    glm::vec3 initialPosition = camera->position;

    // Change target and verify camera position updates accordingly
    controller.setTarget(glm::vec3(5.0f, 0.0f, 0.0f));
    controller.update(0.1f);

    // Camera should have moved but maintained same distance
    EXPECT_NE(camera->position, initialPosition);
    EXPECT_NEAR(glm::length(camera->position - glm::vec3(5.0f, 0.0f, 0.0f)),
                glm::length(initialPosition), 0.1f);
}

// Test vertical movement with space/shift keys
TEST(CameraControllerTest, VerticalMovement) {
    auto camera = std::make_shared<TestCamera>();
    FPSCameraController controller(camera);
    controller.setMovementSpeed(1.0f);

    // Press space to move up
    EXPECT_TRUE(controller.handleInput({InputEvent::Type::KeyDown, SDLK_SPACE}));
    controller.update(1.0f);
    EXPECT_GT(camera->position.y, 0.0f);

    // Press shift to move down
    EXPECT_TRUE(controller.handleInput({InputEvent::Type::KeyUp, SDLK_SPACE}));
    EXPECT_TRUE(controller.handleInput({InputEvent::Type::KeyDown, SDLK_LSHIFT}));

    float prevY = camera->position.y;
    controller.update(1.0f);
    EXPECT_LT(camera->position.y, prevY);
}