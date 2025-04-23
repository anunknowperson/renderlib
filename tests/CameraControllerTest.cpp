#define GLM_ENABLE_EXPERIMENTAL
#include <gtest/gtest.h>
#include <SDL2/SDL.h>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "core/CameraController.h"
#include "scene/Camera.h"

// Mock camera implementation for testing
class MockCamera : public ICamera {
public:
    MockCamera() = default;
    ~MockCamera() override = default;

    void setPosition(const glm::vec3&) override {}
    glm::vec3 getPosition() const override { return {}; }
    void setRotation(const glm::quat&) override {}
    glm::quat getRotation() const override { return glm::quat(); }
    void updateViewport(float, float) override {}
    glm::mat4 getViewMatrix() const override { return glm::mat4(1.0f); }
    glm::mat4 getProjectionMatrix() const override { return glm::mat4(1.0f); }
    glm::vec3 getEulerAngles() const override { return glm::vec3(0.0f); }
};

TEST(CameraControllerTest, HandleKeyInputs) {
    auto camera = std::make_shared<MockCamera>();
    FPSCameraController controller(camera);

    InputEvent keyDown{InputEvent::Type::KeyDown, SDLK_w};
    EXPECT_TRUE(controller.handleInput(keyDown));

    InputEvent keyUp{InputEvent::Type::KeyUp, SDLK_w};
    EXPECT_TRUE(controller.handleInput(keyUp));
}

TEST(CameraControllerTest, HandleMouseInputs) {
    auto camera = std::make_shared<MockCamera>();
    FPSCameraController controller(camera);

    InputEvent mouseDown{InputEvent::Type::MouseDown};
    mouseDown.key = SDL_BUTTON_RIGHT;
    EXPECT_TRUE(controller.handleInput(mouseDown));

    InputEvent mouseUp{InputEvent::Type::MouseUp};
    mouseUp.key = SDL_BUTTON_RIGHT;
    EXPECT_TRUE(controller.handleInput(mouseUp));
}

TEST(CameraControllerTest, OrbitController) {
    auto camera = std::make_shared<MockCamera>();
    OrbitCameraController controller(camera);

    InputEvent wheelEvent{InputEvent::Type::MouseWheel};
    wheelEvent.deltaY = 1;
    EXPECT_TRUE(controller.handleInput(wheelEvent));
}