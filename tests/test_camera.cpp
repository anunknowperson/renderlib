#include <gtest/gtest.h>

#include "SDL2/SDL.h"
#include "glm/glm.hpp"
#include "scene/Camera.h"

// Test camera initialization
TEST(CameraTest, Initialization) {
    Camera camera;
    EXPECT_EQ(camera.position, glm::vec3(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(camera.velocity, glm::vec3(0.0f, 0.0f, 0.0f));
    EXPECT_FLOAT_EQ(camera.pitch, 0.0f);
    EXPECT_FLOAT_EQ(camera.yaw, 0.0f);
}

// Test movement forward when pressing 'W'
TEST(CameraTest, MoveForward) {
    Camera camera;
    SDL_Event event{};
    event.type = SDL_KEYDOWN;
    event.key.keysym.sym = SDLK_w;

    camera.processSDLEvent(event);
    EXPECT_EQ(camera.velocity.z, -1);

    event.type = SDL_KEYUP;
    camera.processSDLEvent(event);
    EXPECT_EQ(camera.velocity.z, 0);
}

// Test movement backward when pressing 'S'
TEST(CameraTest, MoveBackward) {
    Camera camera;
    SDL_Event event{};
    event.type = SDL_KEYDOWN;
    event.key.keysym.sym = SDLK_s;

    camera.processSDLEvent(event);
    EXPECT_EQ(camera.velocity.z, 1);

    event.type = SDL_KEYUP;
    camera.processSDLEvent(event);
    EXPECT_EQ(camera.velocity.z, 0);
}

// Test movement left ('A') and right ('D')
TEST(CameraTest, MoveLeftRight) {
    Camera camera;
    SDL_Event event{};

    event.type = SDL_KEYDOWN;
    event.key.keysym.sym = SDLK_a;
    camera.processSDLEvent(event);
    EXPECT_EQ(camera.velocity.x, -1);

    event.key.keysym.sym = SDLK_d;
    camera.processSDLEvent(event);
    EXPECT_EQ(camera.velocity.x, 1);

    event.type = SDL_KEYUP;
    camera.processSDLEvent(event);
    EXPECT_EQ(camera.velocity.x, 0);
}

// Test if the camera position updates correctly
TEST(CameraTest, UpdatePosition) {
    Camera camera;
    camera.velocity = glm::vec3(1.0f, 0.0f, 0.0f);

    glm::vec3 oldPosition = camera.position;
    camera.update();
    EXPECT_NE(camera.position, oldPosition);
}

// Test if getViewMatrix returns a valid 4x4 matrix
TEST(CameraTest, GetViewMatrix) {
    Camera camera;
    glm::mat4 viewMatrix = camera.getViewMatrix();
    EXPECT_EQ(viewMatrix.length(), 4);  // Should be a 4x4 matrix
}

// Test if getRotationMatrix returns a valid 4x4 matrix
TEST(CameraTest, GetRotationMatrix) {
    Camera camera;
    glm::mat4 rotationMatrix = camera.getRotationMatrix();
    EXPECT_EQ(rotationMatrix.length(), 4);  // Should be a 4x4 matrix
}

// Main function to run the tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
