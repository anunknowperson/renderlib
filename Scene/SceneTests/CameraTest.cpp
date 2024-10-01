#include <gtest/gtest.h>
#include "scene/Camera.h"

class CameraTest : public ::testing::Test {
protected:
    // Метод, выполняемый перед каждым тестом
    void SetUp() override {
        camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), 45.0f, 800.0f, 600.0f);
    }

    Camera camera; // Объект камеры для тестирования
};

TEST_F(CameraTest, SetPosition) {
    camera.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
    EXPECT_EQ(camera.getPosition(), glm::vec3(1.0f, 2.0f, 3.0f));
}

TEST_F(CameraTest, LookAt) {
    camera.lookAt(glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 viewMatrix = camera.getViewMatrix();
    EXPECT_NE(viewMatrix, glm::mat4(1.0f));
}

TEST_F(CameraTest, SetFOV) {
    camera.setFOV(60.0f);
    EXPECT_EQ(camera.getFOV(), 60.0f);
}

TEST_F(CameraTest, DirectionAfterLookAt) {
    camera.lookAt(glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec3 expectedDirection = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f) - camera.getPosition());
    EXPECT_EQ(camera.getDirection(), expectedDirection);
}

TEST_F(CameraTest, UpdateViewMatrixOnPositionChange) {
    camera.setPosition(glm::vec3(0.0f, 0.0f, 10.0f));
    camera.lookAt(glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 viewMatrix = camera.getViewMatrix();
    EXPECT_NE(viewMatrix, glm::mat4(1.0f));
}
