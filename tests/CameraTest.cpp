#include <glm/glm.hpp>
#include <gtest/gtest.h>

#include "core/CameraController.h"
#include "scene/Camera.h"

TEST(CameraTest, TestSetPosition) {
    Camera camera;
    CameraController controller(camera);
    glm::vec3 newPosition(5.0f, 5.0f, 5.0f);
    controller.setPosition(newPosition);

    EXPECT_EQ(controller.getPosition(), newPosition);
}

TEST(CameraTest, TestSetRotation) {
    Camera camera;
    CameraController controller(camera);
    glm::quat newRotation(0.707f, 0.707f, 0.0f, 0.0f);
    controller.setRotation(newRotation);

    EXPECT_EQ(controller.getRotation(), newRotation);
}

TEST(CameraTest, TestSetFOVBoundaries) {
    Camera camera;
    CameraController controller(camera);

    // Проверяем минимальное значение FOV
    controller.setFOV(1.0f);
    EXPECT_FLOAT_EQ(controller.getFOV(), 1.0f);

    // Проверяем максимальное значение FOV
    controller.setFOV(120.0f);
    EXPECT_FLOAT_EQ(controller.getFOV(), 120.0f);
}