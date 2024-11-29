#include "core/ControllerImpl.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <utility>

#define GLM_ENABLE_EXPERIMENTAL
#include <core/View.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <graphics/vulkan/vk_engine.h>
#include <ranges>

ControllerImpl::ControllerImpl(IModel::Ptr model, IView::Ptr view)
    : _model(std::move(model)), _view(std::move(view)), _mesh_controller(_model) {}

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

void updateCube(const MeshController& mesh_controller, Mesh::rid_t rid, int8_t i) {
    const double sinValue = std::sin(getCurrentGlobalTime() + static_cast<double>(i)) * 5.0f;

    const glm::mat4 scale = glm::scale(glm::vec3{0.2});
    const glm::mat4 translation = glm::translate(glm::vec3{static_cast<float>(i) - 2.5f, sinValue, 0});

    mesh_controller.set_transform(rid, scale * translation);
}

void updateCubes(const IModel::Ptr& model, const MeshController& mesh_controller) {
    auto meshes = model->get_meshes();
    for (int8_t i {}; const auto& key : std::views::keys(meshes)) {
        updateCube(mesh_controller, key, i);
        ++i;
    }
}

void update(const IModel::Ptr& model, const MeshController& mesh_controller) {
    model->get_engine().update(model);

    model->getCamera()->update();

    updateCubes(model, mesh_controller);
}

void createCubes(const MeshController& mesh_controller) {
    for (int i = 0; i < 5; i++) {
        mesh_controller.create_mesh("/basicmesh.glb");
    }
}

void ControllerImpl::init() {
    const auto mesh_controller = this->getMeshController();
    createCubes(mesh_controller);

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

            _view->process_event(e);
        }

        // do not draw if we are minimized
        if (stop_rendering) {
            // throttle the speed to avoid the endless spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        _view->run();

        update(_model, mesh_controller);
    }
}


MeshController& ControllerImpl::getMeshController() {
    return _mesh_controller;
}
