#pragma once

#include <SDL_events.h>
#include <memory>

#include "IController.h"
#include "interfaces/IModel.h"
#include "interfaces/IView.h"

class ControllerImpl : public IController {
public:
    ControllerImpl();

    void run() const override;
    void update() const override;
    void processEvent(SDL_Event& e) const override;

private:
    std::shared_ptr<IView> _view;
    std::shared_ptr<IModel> _model;
    void createCubes() const;
};
