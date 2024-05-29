#pragma once

#include "interfaces/IController.h"
#include "interfaces/IModel.h"

IController::Ptr createController(IModel::Ptr ptr);
