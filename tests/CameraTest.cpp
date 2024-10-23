#include <glm/glm.hpp>
#include <gtest/gtest.h>

#include "core/CameraController.h"
#include "scene/Camera.h"

TEST(CameraControllerTest, TestLookAt) {
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), 45.0f, 800.0f, 600.0f);
    CameraController controller(camera);
    glm::vec3 target(0.0f, 0.0f, 0.0f);

    // Проверяем, что поворот камеры меняется корректно при вызове lookAt
    glm::quat initialRotation = controller.getRotation();
    controller.lookAt(target);
    glm::quat newRotation = controller.getRotation();

    EXPECT_NE(initialRotation, newRotation);
}

TEST(CameraControllerTest, TestSetPosition) {
    Camera camera;
    CameraController controller(camera);
    glm::vec3 newPosition(5.0f, 5.0f, 5.0f);
    controller.setPosition(newPosition);

    EXPECT_EQ(controller.getPosition(), newPosition);
}

TEST(CameraControllerTest, TestSetRotation) {
    Camera camera;
    CameraController controller(camera);
    glm::quat newRotation(0.707f, 0.707f, 0.0f, 0.0f);
    controller.setRotation(newRotation);

    EXPECT_EQ(controller.getRotation(), newRotation);
}

TEST(CameraControllerTest, TestSetFOVBoundaries) {
    Camera camera;
    CameraController controller(camera);

    // Проверяем минимальное значение FOV
    controller.setFOV(1.0f);
    EXPECT_FLOAT_EQ(controller.getFOV(), 1.0f);

    // Проверяем максимальное значение FOV
    controller.setFOV(120.0f);
    EXPECT_FLOAT_EQ(controller.getFOV(), 120.0f);
}
