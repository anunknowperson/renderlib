#include <iostream>

#include "core/logging.h"
#include "scene/ParentSystem.h"
#include "scene/MeshSystem.h"
#include "scene/TransformSystem.h"
#include "internal/vulkan_render.h"
#include "graphics/graphics.h"


int main()
{

    flecs::world world;
    VulkanRender r; // Only for testing purposes here

    // Initialize GLFW
    if (!glfwInit())
    {
        LOGE("Failed to initialize GLFW");
        return -1;
    }

    // Set GLFW to not create an OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Create a GLFW window
    GLFWwindow *window = glfwCreateWindow(1280, 720, "Vulkan Test", nullptr, nullptr);


    // Initialize the renderer
    r.init(window);


    if (!window)
    {
        LOGE("Failed to create a GLFW window");
        glfwTerminate();
        return -1;
    }

    while (!glfwWindowShouldClose(window))
    {

        r.render();
        glfwPollEvents();

    }
    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}