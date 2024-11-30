#pragma once

#include <memory>

#include "IController.h"
#include "interfaces/IModel.h"
#include "interfaces/IView.h"

class ViewImpl : public IView {
public:
    explicit ViewImpl(IModel::Ptr model);
    ~ViewImpl() override;

    void run() const override;
    void process_event(const SDL_Event& e) final;

private:
    // IController::Ptr _controller;
    IModel::Ptr _model;

    struct SDL_Window* window{nullptr};
};
