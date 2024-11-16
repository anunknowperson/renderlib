#define GLM_ENABLE_EXPERIMENTAL
#include <SDL2/SDL.h>
#include <gtest/gtest.h>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <glm/gtx/dual_quaternion.hpp>

#include "core/CameraController.h"
#include "scene/Camera.h"

TEST(CameraControllerTest, SetPosition) {
    Camera camera;
    CameraController controller(camera);
    glm::vec3 newPosition(5.0f, 0.0f, 0.0f);

    controller.setPosition(newPosition);
    EXPECT_EQ(controller.getPosition(), newPosition);
}

TEST(CameraControllerTest, SetRotation) {
    Camera camera;
    CameraController controller(camera);
    glm::quat newRotation =
            glm::quat(glm::vec3(0.0f, glm::radians(90.0f), 0.0f));

    controller.setRotation(newRotation);
    EXPECT_EQ(controller.getRotation(), newRotation);
}

TEST(CameraControllerTest, SetPitch) {
    Camera camera;
    CameraController controller(camera);
    float newPitch = 45.0f;

    controller.setPitch(newPitch);
    EXPECT_FLOAT_EQ(controller.getPitch(), newPitch);
}

TEST(CameraControllerTest, SetYaw) {
    Camera camera;
    CameraController controller(camera);
    float newYaw = 90.0f;

    controller.setYaw(newYaw);
    EXPECT_FLOAT_EQ(controller.getYaw(), newYaw);
}

TEST(CameraControllerTest, SetScreenWidth) {
    Camera camera;
    CameraController controller(camera);
    float newScreenWidth = 1920.0f;

    controller.setScreenWidth(newScreenWidth);
    EXPECT_FLOAT_EQ(controller.getScreenWidth(), newScreenWidth);
}

TEST(CameraControllerTest, SetScreenHeight) {
    Camera camera;
    CameraController controller(camera);
    float newScreenHeight = 1080.0f;

    controller.setScreenHeight(newScreenHeight);
    EXPECT_FLOAT_EQ(controller.getScreenHeight(), newScreenHeight);
}

TEST(CameraControllerTest, SetFOV) {
    Camera camera;
    CameraController controller(camera);
    float newFOV = 60.0f;

    controller.setFOV(newFOV);
    EXPECT_FLOAT_EQ(controller.getFOV(), newFOV);
}

TEST(CameraControllerTest, LookAtTarget) {
    Camera camera;
    CameraController controller(camera);
    glm::vec3 target(0.0f, 0.0f, -10.0f);

    controller.lookAt(target);
    EXPECT_TRUE(glm::length(controller.getRotation() -
                            glm::quatLookAt(
                                    glm::normalize(target -
                                                   controller.getPosition()),
                                    glm::vec3(0.0f, 1.0f, 0.0f))) < 0.01f);
}

TEST(CameraControllerTest, ProcessEventMovement) {
    Camera camera;
    CameraController controller(camera);

    SDL_Event event;
    event.type = SDL_KEYDOWN;
    event.key.keysym.sym = SDLK_w;

    controller.processSDLEvent(event);
    EXPECT_EQ(controller.getPosition().z,
              2.9f);  // Предполагаем, что начальная позиция (0, 0, 3)
}

TEST(CameraControllerTest, ProcessEventRotation) {
    Camera camera;
    CameraController controller(camera);

    SDL_Event event;
    event.type = SDL_MOUSEMOTION;
    event.motion.xrel = 10;
    event.motion.yrel = 5;

    std::cout << "Initial Rotation: " << glm::to_string(controller.getRotation()) << std::endl;

    controller.processSDLEvent(event);

    std::cout << "New Rotation: " << glm::to_string(controller.getRotation()) << std::endl;

    EXPECT_GT(controller.getRotation().x, 0.0f);  // Ожидаем, что угол наклона изменился
}

TEST(CameraControllerTest, Update) {
    Camera camera;
    CameraController controller(camera);

    // Assuming updateViewMatrix() modifies some internal state
    controller.update();
    // Add assertions to verify the state change if possible
}