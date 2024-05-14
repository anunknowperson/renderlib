#include <iostream>

#include "core/Logging.h"
#include "scene/ParentSystem.h"
#include "scene/MeshSystem.h"
#include "scene/TransformSystem.h"
#include "graphics/Graphics.h"

#include "graphics/vulkan/vk_engine.h"

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_vulkan.h"

#include <SDL.h>

#include <SDL_vulkan.h>

int main(int argc, char* args[])
{

    // We initialize SDL and create a window with it.
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    struct SDL_Window* window{ nullptr };

    window = SDL_CreateWindow(
            "TODO: PUT APP NAME HERE",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            1700,
            900,
            window_flags);

	VulkanEngine engine;

	engine.init(window);

    SDL_Event e;
    bool bQuit = false;

    bool stop_rendering = false;

    // main loop
    while (!bQuit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // close the window when user alt-f4s or clicks the X button
            if (e.type == SDL_QUIT)
                bQuit = true;

            if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                    stop_rendering = true;
                }
                if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
                    stop_rendering = false;
                }
            }

            ImGui_ImplSDL2_ProcessEvent(&e);
        }

        // do not draw if we are minimized
        if (stop_rendering) {
            // throttle the speed to avoid the endless spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // imgui new frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (ImGui::Begin("background")) {
            ImGui::SliderFloat("Render Scale",&engine.renderScale, 0.3f, 1.f);
            //other code
        }
        ImGui::End();

        //make imgui calculate internal draw structures
        ImGui::Render();

        engine.update();

    }


	engine.cleanup();

    SDL_DestroyWindow(window);

	return 0;

}