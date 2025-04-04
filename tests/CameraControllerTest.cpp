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

    controller.setPosition(glm::vec3(5.f, 0.f, 0.f));
    EXPECT_EQ(controller.getPosition(), glm::vec3(5.f, 0.f, 0.f));
}

TEST(CameraControllerTest, LookAt) {
    Camera camera(glm::vec3(0.f, 0.f, 5.f), 60.f, 800.f, 600.f);
    CameraController controller(camera);

    // Направление камеры на точку (0, 0, 0)
    controller.lookAt(glm::vec3(0.f));
    glm::vec3 forward = camera.getRotation() * glm::vec3(0.f, 0.f, -1.f);
    EXPECT_TRUE(glm::all(glm::epsilonEqual(
        forward,
        glm::normalize(glm::vec3(0.f, 0.f, -1.f)),
        0.001f
    )));
}