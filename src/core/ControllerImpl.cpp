#include "core/ControllerImpl.h"

#include <chrono>
#include <cmath>
#include <string>
#include <thread>
#include <utility>

#include <SDL_events.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <imgui_impl_sdl2.h>

#include "core/Model.h"
#include "core/View.h"
#include "scene/Camera.h"

ControllerImpl::ControllerImpl() : _view(createView()), _model(createModel()) {}

double getCurrentGlobalTime() {
    // Get the current time point
    const auto now = std::chrono::system_clock::now();

    // Cast to a time duration since the epoch
    const auto durationSinceEpoch = now.time_since_epoch();

    // Convert to seconds in double precision
    const std::chrono::duration<double> seconds = durationSinceEpoch;

    // Return the double value
    return seconds.count();
}

void updateCube(const std::shared_ptr<IModel> &_model, int name) {
    const double sinValue = std::sin(getCurrentGlobalTime() + name) * 5.0;

    const glm::mat4 scale = glm::scale(glm::vec3{0.2f});
    const glm::mat4 translation = glm::translate(
            glm::vec3{static_cast<float>(name) - 2.5f, sinValue, 0});

    _model->setMeshTransform("cube" + std::to_string(name),
                             scale * translation);
}

void updateCubes(const std::shared_ptr<IModel> &_model) {
    for (int i = 0; i < 5; i++) {
        updateCube(_model, i);
    }
}

void ControllerImpl::update() const {
    _model->updateVulkan();

    _model->getCamera()->update();

    updateCubes(_model);
}

void ControllerImpl::processEvent(SDL_Event &e) const {
    _model->getCamera()->processSDLEvent(e);
}

void ControllerImpl::createCubes() const {
    for (int i = 0; i < 5; i++) {
        _model->createMesh("cube" + std::to_string(i));
    }
}

void ControllerImpl::run() const {
    createCubes();

    SDL_Event e;
    bool bQuit = false;
    bool stop_rendering = false;

    // main loop
    while (!bQuit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // close the window when user alt-f4s or clicks the X button
            if (e.type == SDL_QUIT) bQuit = true;

            if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                    stop_rendering = true;
                }
                if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
                    stop_rendering = false;
                }
            }

            processEvent(e);
            ImGui_ImplSDL2_ProcessEvent(&e);
        }

        // do not draw if we are minimized
        if (stop_rendering) {
            // throttle the speed to avoid the endless spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        _view->render();
        update();
    }
}
