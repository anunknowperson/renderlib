#pragma once

#include "IController.h"
#include "interfaces/IModel.h"
#include "interfaces/IView.h"

class ControllerImpl : public IController {
public:
    explicit ControllerImpl(IModel::Ptr model, IView::Ptr view);

    void init() override;
    std::weak_ptr<const MeshController> getMeshController() final;

private:
    IModel::Ptr _model;
    IView::Ptr _view;
    std::shared_ptr<MeshController> _mesh_controller;
};
