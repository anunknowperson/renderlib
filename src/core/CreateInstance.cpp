#include "IController.h"
#include "core/Controller.h"
#include "core/Model.h"
#include "interfaces/IModel.h"

IController::Ptr createInstance() {
    const auto model = createModel();
    const auto controller = createController(model);
    return controller;
}
