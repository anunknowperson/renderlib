#include <cassert>
#include <chrono>
#include <iostream>
#include <utility>

#include "core/ControllerImpl.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

ControllerImpl::ControllerImpl(IModel::Ptr model)
    : _model(std::move(model)) {
}

double getCurrentGlobalTime() {
    // Get the current time point
    auto now = std::chrono::system_clock::now();

    // Cast to a time duration since the epoch
    auto durationSinceEpoch = now.time_since_epoch();

    // Convert to seconds in double precision
    std::chrono::duration<double> seconds = durationSinceEpoch;

    // Return the double value
    return seconds.count();
}

void createCubes(const std::shared_ptr<IModel> &_model) {
    for (int i = 0; i < 5; i++) {
        _model->createMesh("cube" + i);
    }
}

void updateCube(const std::shared_ptr<IModel> &_model, int name) {
    double sinValue = std::sin(getCurrentGlobalTime() + name) * 5.0f;


    glm::mat4 scale = glm::scale(glm::vec3{0.2});
    glm::mat4 translation = glm::translate(glm::vec3{name - 2.5f, sinValue, 0});


    _model->setMeshTransform("cube" + name, scale * translation);
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

void ControllerImpl::init() const {

    createCubes(_model);
}
