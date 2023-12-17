#include <iostream>

#include "core/logging.h"

#include "internal/vulkan_render.h"
#include "graphics/graphics.h"

int main() {
    engine::graphics::Graphics g;

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


    uint64_t obj1 = engine::graphics::Graphics::get_instance()->create_mesh_instance();
    uint64_t obj2 = engine::graphics::Graphics::get_instance()->create_mesh_instance();
    uint64_t obj3 = engine::graphics::Graphics::get_instance()->create_mesh_instance();

    engine::graphics::Graphics::get_instance()->free_mesh_instance(obj2);
    engine::graphics::Graphics::get_instance()->free_mesh_instance(obj1);

    uint64_t obj4 = engine::graphics::Graphics::get_instance()->create_mesh_instance();

    while (!glfwWindowShouldClose(window)) {

        engine::graphics::Graphics::get_instance()->set_mesh_instance_transform(obj3, glm::translate(glm::mat4(1.0), glm::vec3(2.0, 0.0, 0.0)));
        engine::graphics::Graphics::get_instance()->set_mesh_instance_transform(obj4, glm::translate(glm::mat4(1.0), glm::vec3(-2.0, 0.0, 0.5)));

        r.render();
        
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    

    


    
    return 0;
}

