#pragma once

#include <memory>

#include "interfaces/IModel.h"
#include "interfaces/IView.h"

class ViewImpl final : public IView {
public:
    explicit ViewImpl(IModel::Ptr model);
    ~ViewImpl() override;

    void run() const override;
    void process_event(const SDL_Event& e) final;

private:
    IModel::Ptr _model;

    SDL_Window* window{nullptr};
};
