#include <charconv>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <thread>
#include <utility>
#include <variant>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"
#include "imgui.h"

#include "core/ViewImpl.h"
#include "graphics/vulkan/vk_engine.h"


ViewImpl::ViewImpl(IController::Ptr controller, IModel::Ptr model) : _controller(std::move(controller)),
                                                                     _model(std::move(model)) {

    SDL_Init(SDL_INIT_VIDEO);

    auto window_flags = (SDL_WindowFlags) (SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);


    window = SDL_CreateWindow(
            "engine",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            1700,
            900,
            window_flags);
}

void createCubes(const std::shared_ptr<IModel> &_model) {
    for (int i = 0; i < 5; i++) {
        _model->createMesh("cube" + i);
    }
}

void ViewImpl::run() const {

    _model->registerWindow(window);

    createCubes(_model);

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

            _controller->processEvent(e);
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

            VulkanEngine &engine = VulkanEngine::Get();
            ImGui::SliderFloat("Render Scale", &engine.renderScale, 0.3f, 1.f);
            //other code
        }
        ImGui::End();

        //make imgui calculate internal draw structures
        ImGui::Render();


        _controller->update();
        //_model->updateVulkan();

        //engine.update();
    }
}

ViewImpl::~ViewImpl() {
    SDL_DestroyWindow(window);
}
