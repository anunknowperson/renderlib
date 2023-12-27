#include <iostream>

#include "core/Logging.h"
#include "scene/ParentSystem.h"
#include "scene/MeshSystem.h"
#include "scene/TransformSystem.h"
#include "graphics/drivers/vulkan/VulkanRender.h"
#include "graphics/Graphics.h"


int main()
{

    flecs::world world;
    VulkanRender r;

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