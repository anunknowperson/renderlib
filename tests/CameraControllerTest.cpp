#define GLM_ENABLE_EXPERIMENTAL
#include <SDL2/SDL.h>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <gtest/gtest.h>
#include <iostream>

#include "core/CameraController.h"
#include "scene/Camera.h"

TEST(CameraControllerTest, SetPosition) {
    Camera camera;
    CameraController controller(camera);
    glm::vec3 newPosition(5.0f, 0.0f, 0.0f);

    controller.setPosition(newPosition);
    EXPECT_EQ(camera.getPosition(), newPosition);
}

TEST(CameraControllerTest, SetRotation) {
    Camera camera;
    CameraController controller(camera);
    glm::quat newRotation =
            glm::quat(glm::vec3(0.0f, glm::radians(90.0f), 0.0f));

    controller.setRotation(newRotation);
    EXPECT_EQ(camera.getRotation(), newRotation);
}

TEST(CameraControllerTest, LookAtTarget) {
    Camera camera;
    CameraController controller(camera);
    glm::vec3 target(0.0f, 0.0f, -10.0f);

    controller.lookAt(target);
    EXPECT_TRUE(
            glm::length(camera.getRotation() -
                        glm::quatLookAt(
                                glm::normalize(target - camera.getPosition()),
                                glm::vec3(0.0f, 1.0f, 0.0f))) < 0.01f);
}

TEST(CameraControllerTest, ProcessEventMovement) {
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), 45.0f, 800.0f, 600.0f);
    CameraController controller(camera);

    SDL_Event event;
    event.type = SDL_KEYDOWN;
    event.key.keysym.sym = SDLK_w;  // Нажимаем клавишу W

    controller.processSDLEvent(event);

    EXPECT_FLOAT_EQ(camera.getPosition().z,
                    2.9f);  // Проверяем смещение назад по Z
}

TEST(CameraControllerTest, ProcessEventRotation) {
    Camera camera;
    CameraController controller(camera);

    // Set expected rotation values
    glm::quat expectedRotation = glm::quat(glm::vec3(0.0f, 0.0044f, -0.0088f));
    controller.setRotation(expectedRotation);

    // Get the actual rotation from the controller
    glm::quat actualRotation = controller.getRotation();

    // Compare the rotation values
    EXPECT_NEAR(actualRotation.x, expectedRotation.x, 0.001f);
    EXPECT_NEAR(actualRotation.y, expectedRotation.y, 0.001f);
    EXPECT_NEAR(actualRotation.z, expectedRotation.z, 0.001f);
    EXPECT_NEAR(actualRotation.w, expectedRotation.w, 0.001f);
}

// Вспомогательная функция для эмуляции нажатия клавиш
void triggerKeyPress(CameraController& controller, SDL_Keycode key) {
    SDL_Event event;
    event.type = SDL_KEYDOWN;
    event.key.keysym.sym = key;
    controller.processSDLEvent(event);
}

TEST(CameraControllerTest, ProcessAllMovementKeys) {
    Camera camera(glm::vec3(0.0f), 45.0f, 800.0f, 600.0f);
    CameraController controller(camera);
    float moveSpeed = 0.1f;

    // Движение вперед (W)
    triggerKeyPress(controller, SDLK_w);
    EXPECT_EQ(camera.getPosition().z, -moveSpeed);

    // Движение назад (S)
    triggerKeyPress(controller, SDLK_s);
    EXPECT_EQ(camera.getPosition().z, 0.0f);

    // Движение влево (A)
    triggerKeyPress(controller, SDLK_a);
    EXPECT_EQ(camera.getPosition().x, -moveSpeed);

    // Движение вправо (D)
    triggerKeyPress(controller, SDLK_d);
    EXPECT_EQ(camera.getPosition().x, 0.0f);
}

TEST(CameraControllerTest, CombinedMovement) {
    Camera camera(glm::vec3(0.0f), 45.0f, 800.0f, 600.0f);
    CameraController controller(camera);
    float moveSpeed = 0.1f;

    // Нажать W и D одновременно
    triggerKeyPress(controller, SDLK_w);
    triggerKeyPress(controller, SDLK_d);

    EXPECT_EQ(camera.getPosition().x, moveSpeed);
    EXPECT_EQ(camera.getPosition().z, -moveSpeed);
}

// Вспомогательная функция для эмуляции движения мыши
void simulateMouseMovement(CameraController& controller, int xrel, int yrel) {
    SDL_Event event;
    event.type = SDL_MOUSEMOTION;
    event.motion.xrel = xrel;
    event.motion.yrel = yrel;
    controller.processSDLEvent(event);
}

TEST(CameraControllerTest, PitchClamping) {
    Camera camera;
    CameraController controller(camera);
    float sensitivity = 0.0001f;

    // Попытка превысить верхний предел pitch (+89°)
    simulateMouseMovement(controller, 0, 1000000); // Огромное смещение по Y
    EXPECT_LE(camera.getPitch(), glm::radians(89.0f));

    // Попытка опуститься ниже нижнего предела (-89°)
    simulateMouseMovement(controller, 0, -1000000);
    EXPECT_GE(camera.getPitch(), glm::radians(-89.0f));
}

TEST(CameraControllerIntegrationTest, PositionAndRotationSync) {
    Camera camera;
    CameraController controller(camera);

    // Изменение позиции через контроллер
    glm::vec3 newPos(1.0f, 2.0f, 3.0f);
    controller.setPosition(newPos);
    EXPECT_EQ(camera.getPosition(), newPos);

    // Изменение вращения через контроллер
    glm::quat newRot = glm::angleAxis(glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    controller.setRotation(newRot);
    EXPECT_EQ(camera.getRotation(), newRot);
}