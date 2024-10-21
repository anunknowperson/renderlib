#include <gtest/gtest.h>
#include "core/CameraController.h"
#include "scene/Camera.h"

// Проверка установки новой позиции камеры
TEST(CameraControllerTest, SetPosition) {
    Camera camera;
    CameraController controller(camera);
    glm::vec3 newPosition(5.0f, 0.0f, 0.0f);

    controller.setPosition(newPosition);
    EXPECT_EQ(camera.getPosition(), newPosition);
}

// Проверка установки нового вращения камеры
TEST(CameraControllerTest, SetRotation) {
    Camera camera;
    CameraController controller(camera);
    glm::quat newRotation = glm::quat(glm::vec3(0.0f, glm::radians(90.0f), 0.0f));

    controller.setRotation(newRotation);
    EXPECT_EQ(camera.getRotation(), newRotation);
}

// Проверка метода LookAt, который настраивает камеру для наблюдения за целью
TEST(CameraControllerTest, LookAtTarget) {
    Camera camera;
    CameraController controller(camera);
    glm::vec3 target(0.0f, 0.0f, -10.0f);

    controller.lookAt(target);
    // Проверяем, что камера корректно "смотрит" на цель с допуском в 0.01
    EXPECT_TRUE(glm::length(camera.getRotation() - glm::quatLookAt(glm::normalize(target - camera.getPosition()), glm::vec3(0.0f, 1.0f, 0.0f))) < 0.01f);
}

// Проверка установки нового значения угла обзора (FOV) для камеры
TEST(CameraControllerTest, SetFOV) {
    Camera camera;
    CameraController controller(camera);
    float newFOV = 60.0f;

    controller.setFOV(newFOV);
    EXPECT_FLOAT_EQ(camera.getFOV(), newFOV);
}

// Проверка обработки события движения (W - вперед)
TEST(CameraControllerTest, ProcessEventMovement) {
    Camera camera;
    CameraController controller(camera);

    SDL_Event event;
    event.type = SDL_KEYDOWN;
    event.key.keysym.sym = SDLK_w;

    controller.processSDLEvent(event);
    // Ожидаем, что позиция камеры изменится по оси Z (движение вперед)
    EXPECT_EQ(camera.getPosition().z, 2.9f);  // Предполагаем, что начальная позиция (0, 0, 3)
}

// Проверка обработки события вращения (движение мышью)
TEST(CameraControllerTest, ProcessEventRotation) {
    Camera camera;
    CameraController controller(camera);

    SDL_Event event;
    event.type = SDL_MOUSEMOTION;
    event.motion.xrel = 10;
    event.motion.yrel = 5;

    controller.processSDLEvent(event);
    // Проверяем, что вращение камеры изменилось после движения мыши
    EXPECT_GT(camera.getRotation().x, 0.0f);  // Ожидаем, что угол наклона изменился
}

// Проверка конструктора камеры по умолчанию и правильности установки начальных значений
TEST(CameraTest, DefaultConstructor) {
    Camera camera;

    EXPECT_EQ(camera.getPosition(), glm::vec3(0.0f, 0.0f, 3.0f));  // Проверяем начальную позицию
    EXPECT_EQ(camera.getRotation(), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));  // Проверяем начальное вращение
    EXPECT_FLOAT_EQ(camera.getFOV(), 45.0f);  // Проверяем начальный FOV
    EXPECT_FLOAT_EQ(camera.getWidth(), 800.0f);  // Проверяем начальную ширину
    EXPECT_FLOAT_EQ(camera.getHeight(), 600.0f);  // Проверяем начальную высоту
}

// Проверка изменения ширины и высоты камеры
TEST(CameraTest, TestSetWidthHeight) {
    Camera camera;
    camera.setWidth(1920.0f);
    camera.setHeight(1080.0f);

    // Проверяем, что значения ширины и высоты обновились
    EXPECT_FLOAT_EQ(camera.getWidth(), 1920.0f);
    EXPECT_FLOAT_EQ(camera.getHeight(), 1080.0f);
}

// Проверка изменения матрицы вида после изменения позиции камеры
TEST(CameraTest, TestViewMatrix) {
    Camera camera;
    camera.setPosition(glm::vec3(1.0f, 1.0f, 1.0f));

    // Сохраняем начальное состояние матрицы вида
    glm::mat4 initialViewMatrix = camera.getViewMatrix();
    camera.setPosition(glm::vec3(2.0f, 2.0f, 2.0f));

    // Проверяем, что матрица вида изменилась после перемещения камеры
    EXPECT_NE(camera.getViewMatrix(), initialViewMatrix);
}
