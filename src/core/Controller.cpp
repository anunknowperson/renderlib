#include "core/Controller.h"

#include <memory>
#include <utility>

#include "core/ControllerImpl.h"

IController::Ptr createController() {
    return std::make_shared<ControllerImpl>();
}