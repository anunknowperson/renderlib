#include <gtest/gtest.h>
#include "scene/Camera.h"
#include <glm/glm.hpp>

TEST(CameraTest, GetSetPosition) {
    Camera camera;
    glm::vec3 newPos(1.0f, 2.0f, 3.0f);
    camera.setPosition(newPos);
    EXPECT_EQ(camera.getPosition(), newPos);
}

TEST(CameraTest, GetSetRotation) {
    Camera camera;
    glm::quat newRotation = glm::quat(glm::vec3(0.0f, 0.0f, glm::radians(45.0f)));
    camera.setRotation(newRotation);
    EXPECT_EQ(camera.getRotation(), newRotation);
}

TEST(CameraTest, GetSetFOV) {
    Camera camera;
    float newFOV = 90.0f;
    camera.setFOV(newFOV);
    EXPECT_FLOAT_EQ(camera.getFOV(), newFOV);
}
