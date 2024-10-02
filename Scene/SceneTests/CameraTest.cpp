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
    // Для сравнения матриц лучше использовать библиотеку, которая поддерживает такую проверку, или хотя бы проверить отдельные элементы
    EXPECT_NE(viewMatrix, glm::mat4(1.0f)); // Проверка, что матрица изменилась
}

TEST_F(CameraTest, SetFOV) {
    camera.setFOV(60.0f);
    EXPECT_EQ(camera.getFOV(), 60.0f);
}

TEST_F(CameraTest, DirectionAfterLookAt) {
    camera.lookAt(glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec3 expectedDirection = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f) - camera.getPosition());
    // Используем EXPECT_NEAR для сравнения компонентов вектора с допуском
    EXPECT_NEAR(camera.getDirection().x, expectedDirection.x, 1e-5);
    EXPECT_NEAR(camera.getDirection().y, expectedDirection.y, 1e-5);
    EXPECT_NEAR(camera.getDirection().z, expectedDirection.z, 1e-5);
}

TEST_F(CameraTest, UpdateViewMatrixOnPositionChange) {
    camera.setPosition(glm::vec3(0.0f, 0.0f, 10.0f));
    camera.lookAt(glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 viewMatrix = camera.getViewMatrix();
    EXPECT_NE(viewMatrix, glm::mat4(1.0f)); // Проверка, что матрица изменилась
}
