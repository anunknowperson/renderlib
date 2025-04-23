#define GLM_ENABLE_EXPERIMENTAL
#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "scene/Camera.h"

TEST(CameraTest, Initialization) {
    Camera defaultCamera;
    EXPECT_EQ(defaultCamera.getPosition(), glm::vec3(0.f));
    EXPECT_FLOAT_EQ(defaultCamera.getFOV(), 45.f);
    EXPECT_FLOAT_EQ(defaultCamera.getScreenWidth(), 800.f);
    EXPECT_FLOAT_EQ(defaultCamera.getScreenHeight(), 600.f);

    Camera customCamera(
        glm::vec3(2.f, 3.f, 5.f),
        70.f,
        1920.f,
        1080.f,
        0.5f,
        2000.f
    );
    EXPECT_EQ(customCamera.getPosition(), glm::vec3(2.f, 3.f, 5.f));
    EXPECT_FLOAT_EQ(customCamera.getFOV(), 70.f);
    EXPECT_FLOAT_EQ(customCamera.getScreenWidth(), 1920.f);
    EXPECT_FLOAT_EQ(customCamera.getScreenHeight(), 1080.f);
}

TEST(CameraTest, ViewMatrix) {
    Camera camera(glm::vec3(0.f), 45.f, 800.f, 600.f);

    // Проверка начальной матрицы
    glm::mat4 expectedView = glm::lookAt(
        glm::vec3(0.f),
        glm::vec3(0.f, 0.f, -1.f),
        glm::vec3(0.f, 1.f, 0.f)
    );

    // Сравнение каждого элемента матрицы с точностью 0.001
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_NEAR(camera.getViewMatrix()[i][j], expectedView[i][j], 0.001f);
        }
    }

    // Изменение позиции
    camera.setPosition(glm::vec3(0.f, 0.f, 5.f));
    expectedView = glm::lookAt(
        glm::vec3(0.f, 0.f, 5.f),
        glm::vec3(0.f, 0.f, 4.f),
        glm::vec3(0.f, 1.f, 0.f)
    );

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_NEAR(camera.getViewMatrix()[i][j], expectedView[i][j], 0.001f);
        }
    }

    // Изменение вращения
    camera.setEulerAngles(glm::vec3(0.f, 90.f, 0.f));
    expectedView = glm::lookAt(
        glm::vec3(0.f, 0.f, 5.f),
        glm::vec3(1.f, 0.f, 5.f),
        glm::vec3(0.f, 1.f, 0.f)
    );

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_NEAR(camera.getViewMatrix()[i][j], expectedView[i][j], 0.001f);
        }
    }
}

TEST(CameraTest, ForwardVector) {
    Camera camera;
    camera.setEulerAngles(glm::vec3(0.f, 90.f, 0.f));
    glm::vec3 forward = camera.getRotation() * glm::vec3(0.f, 0.f, -1.f);
    EXPECT_TRUE(glm::all(glm::epsilonEqual(
        forward,
        glm::vec3(1.f, 0.f, 0.f),
        0.001f
    )));
}

TEST(CameraTest, ProjectionMatrix) {
    Camera camera(glm::vec3(0.f), 60.f, 800.f, 600.f);
    glm::mat4 expectedProj = glm::perspective(
        glm::radians(60.f),
        800.f / 600.f,
        0.1f,
        10000.f
    );
    expectedProj[1][1] *= -1;

    EXPECT_EQ(camera.getProjectionMatrix(), expectedProj);

    // Изменение FOV
    camera.setFOV(90.f);
    expectedProj = glm::perspective(
        glm::radians(90.f),
        800.f / 600.f,
        0.1f,
        10000.f
    );
    expectedProj[1][1] *= -1;
    EXPECT_EQ(camera.getProjectionMatrix(), expectedProj);
}

TEST(CameraTest, SettersAndGetters) {
    Camera camera;

    // Позиция
    camera.setPosition(glm::vec3(1.f, 2.f, 3.f));
    EXPECT_EQ(camera.getPosition(), glm::vec3(1.f, 2.f, 3.f));

    // Вращение через углы Эйлера
    camera.setEulerAngles(glm::vec3(45.f, 30.f, 0.f));
    glm::vec3 angles = camera.getEulerAngles();
    EXPECT_NEAR(angles.x, 45.f, 0.001f);
    EXPECT_NEAR(angles.y, 30.f, 0.001f);
    EXPECT_NEAR(angles.z, 0.f, 0.001f);

    // Разрешение экрана
    camera.setScreenWidth(1280.f);
    camera.setScreenHeight(720.f);
    EXPECT_FLOAT_EQ(camera.getScreenWidth(), 1280.f);
    EXPECT_FLOAT_EQ(camera.getScreenHeight(), 720.f);
}