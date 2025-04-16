#include <gtest/gtest.h>
#include <memory>
#include "core/CameraController.h"
#include "scene/Camera.h"
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

TEST(CameraControllerTest, Initialization) {
    Camera camera(glm::vec3(0.0f), 45.0f, 800.0f, 600.0f, 0.1f, 1000.0f);
    CameraController controller(camera);

    EXPECT_FLOAT_EQ(controller.getMovementSpeed(), 0.0f);
    EXPECT_FLOAT_EQ(controller.getRotationSpeed(), 0.0f);
}

TEST(CameraControllerTest, MovementInput) {
    Camera camera(glm::vec3(0.0f), 45.0f, 800.0f, 600.0f, 0.1f, 1000.0f);
    CameraController controller(camera);
    controller.setMovementSpeed(5.0f);

    // Create input event using correct Type:: prefix
    InputEvent forwardEvent;
    forwardEvent.type = InputEvent::Type::KeyDown;
    forwardEvent.key = SDLK_w;

    EXPECT_TRUE(controller.handleInput(forwardEvent));
    controller.update(1.0f);

    glm::vec3 position = camera.getPosition();
    EXPECT_NEAR(position.z, -5.0f, 0.001f);
}

TEST(CameraControllerTest, RotationInput) {
    Camera camera(glm::vec3(0.0f), 45.0f, 800.0f, 600.0f, 0.1f, 1000.0f);
    CameraController controller(camera);
    controller.setRotationSpeed(0.1f);

    // Activate rotation mode with right mouse button
    InputEvent mouseDownEvent;
    mouseDownEvent.type = InputEvent::Type::MouseDown;
    mouseDownEvent.key = SDL_BUTTON_RIGHT;
    mouseDownEvent.x = 100;
    mouseDownEvent.y = 100;
    EXPECT_TRUE(controller.handleInput(mouseDownEvent));

    // Simulate mouse movement
    InputEvent mouseMoveEvent;
    mouseMoveEvent.type = InputEvent::Type::MouseMove;
    mouseMoveEvent.x = 110;
    mouseMoveEvent.y = 95;
    mouseMoveEvent.x = 110;
    mouseMoveEvent.y = 95;
    EXPECT_TRUE(controller.handleInput(mouseMoveEvent));

    glm::quat originalRotation(1.0f, 0.0f, 0.0f, 0.0f);
    EXPECT_NE(camera.getRotation(), originalRotation);
}