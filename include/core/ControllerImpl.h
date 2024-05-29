#pragma once

#include "interfaces/IController.h"
#include "interfaces/IModel.h"

class ControllerImpl : public IController {
public:
    explicit ControllerImpl(IModel::Ptr model);

    void init() const override;
    void update() const override;
    void processEvent(SDL_Event &e) const override;

private:
    std::shared_ptr<IModel> _model;
};
