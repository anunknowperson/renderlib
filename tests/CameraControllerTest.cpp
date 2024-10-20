#include <gtest/gtest.h>
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
    glm::quat newRotation = glm::quat(glm::vec3(0.0f, glm::radians(90.0f), 0.0f));

    controller.setRotation(newRotation);
    EXPECT_EQ(camera.getRotation(), newRotation);
}

TEST(CameraControllerTest, LookAtTarget) {
    Camera camera;
    CameraController controller(camera);
    glm::vec3 target(0.0f, 0.0f, -10.0f);

    controller.lookAt(target);
    EXPECT_TRUE(glm::length(camera.getRotation() - glm::quatLookAt(glm::normalize(target - camera.getPosition()), glm::vec3(0.0f, 1.0f, 0.0f))) < 0.01f);
}

TEST(CameraControllerTest, SetFOV) {
    Camera camera;
    CameraController controller(camera);
    float newFOV = 60.0f;

    controller.setFOV(newFOV);
    EXPECT_FLOAT_EQ(camera.getFOV(), newFOV);
}

TEST(CameraControllerTest, ProcessEventMovement) {
    Camera camera;
    CameraController controller(camera);

    SDL_Event event;
    event.type = SDL_KEYDOWN;
    event.key.keysym.sym = SDLK_w;

    controller.processSDLEvent(event);
    EXPECT_EQ(camera.getPosition().z, 2.9f);  // Assuming initial position is (0, 0, 3)
}

TEST(CameraControllerTest, ProcessEventRotation) {
    Camera camera;
    CameraController controller(camera);

    SDL_Event event;
    event.type = SDL_MOUSEMOTION;
    event.motion.xrel = 10;
    event.motion.yrel = 5;

    controller.processSDLEvent(event);
    EXPECT_GT(camera.getRotation().x, 0.0f);  // Проверяем, что камера изменила вращение
}
