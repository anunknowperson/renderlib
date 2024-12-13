#include <memory>
#include <utility>

#include "core/Controller.h"

#include "core/ControllerImpl.h"

IController::Ptr createController(IModel::Ptr ptr) {
    return std::make_shared<ControllerImpl>(std::move(ptr));
}