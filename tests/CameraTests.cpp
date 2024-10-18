#include <../gtest.h>
#include "scene/Camera.h"

TEST(CameraTest, DefaultConstructor) {
Camera camera;
EXPECT_EQ(camera.getPosition(), glm::vec3(0.0f, 0.0f, 3.0f));
EXPECT_EQ(camera.getFOV(), 45.0f);
}

TEST(CameraTest, ParameterizedConstructor) {
Camera camera(glm::vec3(1.0f, 2.0f, 3.0f), 60.0f, 800.0f, 600.0f);
EXPECT_EQ(camera.getPosition(), glm::vec3(1.0f, 2.0f, 3.0f));
EXPECT_EQ(camera.getFOV(), 60.0f);
}
