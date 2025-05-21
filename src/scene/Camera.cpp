#define GLM_ENABLE_EXPERIMENTAL
#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "scene/Camera.h"

TEST(CameraTest, Initialization) {
    Camera defaultCamera;
    EXPECT_EQ(defaultCamera.getPosition(), glm::vec3(0.f));
    EXPECT_FLOAT_EQ(defaultCamera.getFOV(), 45.f);

    // The default aspect ratio is 800/600
    EXPECT_NEAR(defaultCamera.getScreenWidth() / defaultCamera.getScreenHeight(),
                800.f / 600.f, 0.001f);

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
    EXPECT_NEAR(customCamera.getScreenWidth() / customCamera.getScreenHeight(),
                1920.f / 1080.f, 0.001f);
}

TEST(CameraTest, ViewMatrix) {
    Camera camera(glm::vec3(0.f), 45.f, 800.f, 600.f, 0.1f, 10000.f);

    // Test default camera orientation - should look down negative Z axis
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::vec4 forward = viewMatrix * glm::vec4(0.f, 0.f, -1.f, 0.f);
    EXPECT_NEAR(forward.z, -1.f, 0.001f);

    // Move camera to position (0, 0, 5)
    camera.setPosition(glm::vec3(0.f, 0.f, 5.f));
    viewMatrix = camera.getViewMatrix();

    // Origin should be at (0, 0, -5) in view space
    glm::vec4 origin = viewMatrix * glm::vec4(0.f, 0.f, 0.f, 1.f);
    EXPECT_NEAR(origin.z, -5.f, 0.001f);

    // Rotate camera 90 degrees around Y
    camera.setEulerAngles(glm::vec3(0.f, 90.f, 0.f));
    viewMatrix = camera.getViewMatrix();

    // After rotation, looking down X axis
    glm::vec4 lookDirection = inverse(viewMatrix) * glm::vec4(0.f, 0.f, -1.f, 0.f);
    EXPECT_NEAR(lookDirection.x, 1.f, 0.001f);
    EXPECT_NEAR(lookDirection.z, 0.f, 0.001f);
}

TEST(CameraTest, ForwardVector) {
    Camera camera;
    camera.setEulerAngles(glm::vec3(0.f, 90.f, 0.f));
    glm::vec3 forward = camera.getRotation() * glm::vec3(0.f, 0.f, -1.f);

    // When rotated 90 degrees around Y, forward should point along positive X
    EXPECT_NEAR(forward.x, 1.f, 0.001f);
    EXPECT_NEAR(forward.y, 0.f, 0.001f);
    EXPECT_NEAR(forward.z, 0.f, 0.001f);
}

TEST(CameraTest, ProjectionMatrix) {
    // Create camera with specific parameters
    Camera camera(glm::vec3(0.f), 60.f, 800.f, 600.f, 0.1f, 10000.f);

    // Create the expected projection matrix with the same parameters
    glm::mat4 expectedProj = glm::perspective(
        glm::radians(60.f),
        800.f / 600.f,
        0.1f,
        10000.f
    );
    expectedProj[1][1] *= -1; // Flip for Vulkan

    // Compare matrices with an element-wise check to handle floating point precision
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_NEAR(camera.getProjectionMatrix()[i][j], expectedProj[i][j], 0.001f);
        }
    }

    // Test changing FOV
    camera.setFOV(90.f);
    expectedProj = glm::perspective(
        glm::radians(90.f),
        800.f / 600.f,
        0.1f,
        10000.f
    );
    expectedProj[1][1] *= -1;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_NEAR(camera.getProjectionMatrix()[i][j], expectedProj[i][j], 0.001f);
        }
    }
}

TEST(CameraTest, SettersAndGetters) {
    Camera camera;

    // Position
    camera.setPosition(glm::vec3(1.f, 2.f, 3.f));
    EXPECT_EQ(camera.getPosition(), glm::vec3(1.f, 2.f, 3.f));

    // Rotation via Euler angles
    camera.setEulerAngles(glm::vec3(45.f, 30.f, 0.f));
    glm::vec3 angles = camera.getEulerAngles();
    EXPECT_NEAR(angles.x, 45.f, 0.001f);
    EXPECT_NEAR(angles.y, 30.f, 0.001f);
    EXPECT_NEAR(angles.z, 0.f, 0.001f);

    // Screen dimensions
    camera.setScreenWidth(1280.f);
    camera.setScreenHeight(720.f);
    EXPECT_NEAR(camera.getScreenWidth() / camera.getScreenHeight(), 1280.f / 720.f, 0.001f);
}