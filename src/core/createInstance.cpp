#include "IController.h"
#include "core/Controller.h"
#include "core/Model.h"

IController::Ptr createInstance() {

    const auto controller = createController();
    return controller;
}
