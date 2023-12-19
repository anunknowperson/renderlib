#include <iostream>

#include "core/logging.h"
#include "scene/ParentSystem.h"
#include "scene/mesh_system.h"
#include "scene/TransformSystem.h"
#include "internal/vulkan_render.h"
#include "graphics/graphics.h"


int main() {
    engine::graphics::Graphics g;
    flecs::world world;
    VulkanRender r; // Only for testing purposes here

    // Initialize GLFW
    if (!glfwInit()) {
        LOGE("Failed to initialize GLFW");
        return -1;
    }

    // Set GLFW to not create an OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Vulkan Test", nullptr, nullptr);


    // Initialize the renderer
    r.init(window);


    if (!window) {
        LOGE("Failed to create a GLFW window");
        glfwTerminate();
        return -1;
    }



    world.set<flecs::Rest>({});
    world.component<Parent>();
    world.component<Child>();
    world.component<PreviousParent>();
    world.component<GlobalTransform>();
    world.component<LocalTransform>();
    world.component<MeshComponent>();

    HierarchySystem(world);
    TransformSystem(world);
    MeshSystem(world);

    flecs::entity object1 = world.entity("object1");
    //flecs::entity object2 = world.entity("object2");


    while (!glfwWindowShouldClose(window)) {
        if (!object1.has<GlobalTransform>()) {
            SetGlobalFromPosition(object1, glm::vec3(0.0, 0.0, 0.0));
            auto mat = object1.get<GlobalTransform>()->TransformMatrix;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    std::cout << mat[i][j] << ' ';
                }
                std::cout << '\n';
            };
        }
        world.progress();
        r.render();
        glfwPollEvents();

        if (object1.get<GlobalTransform>()->TransformMatrix == glm::f64mat4(1.0)) {
            SetGlobalFromPosition(object1, glm::vec3(0.5, 0.5, 1.0));
            auto mat = object1.get<GlobalTransform>()->TransformMatrix;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    std::cout << mat[i][j] << ' ';
                }
                std::cout << '\n';
            };
        }
    }
    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

