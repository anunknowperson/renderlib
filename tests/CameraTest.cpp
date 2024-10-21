#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "scene/Camera.h"

TEST(CameraTest, TestLookAt) {
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), 45.0f, 800.0f, 600.0f);
    glm::vec3 target(0.0f, 0.0f, 0.0f);

    // Проверяем, что поворот камеры меняется корректно при вызове lookAt
    glm::quat initialRotation = camera.getRotation();
    camera.lookAt(target);
    glm::quat newRotation = camera.getRotation();

    EXPECT_NE(initialRotation, newRotation);
}

TEST(CameraTest, TestSetPosition) {
    Camera camera;
    glm::vec3 newPosition(5.0f, 5.0f, 5.0f);
    camera.setPosition(newPosition);

    EXPECT_EQ(camera.getPosition(), newPosition);
}

TEST(CameraTest, TestSetRotation) {
    Camera camera;
    glm::quat newRotation(0.707f, 0.707f, 0.0f, 0.0f);
    camera.setRotation(newRotation);

    EXPECT_EQ(camera.getRotation(), newRotation);
}

TEST(CameraTest, TestSetFOVBoundaries) {
    Camera camera;

    // Проверяем минимальное значение FOV
    camera.setFOV(1.0f);
    EXPECT_FLOAT_EQ(camera.getFOV(), 1.0f);

    // Проверяем максимальное значение FOV
    camera.setFOV(120.0f);
    EXPECT_FLOAT_EQ(camera.getFOV(), 120.0f);
}
