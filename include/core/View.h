#pragma once

#include "interfaces/IController.h"
#include "interfaces/IModel.h"
#include "interfaces/IView.h"

IView::Ptr createView(IController::Ptr controller, IModel::Ptr model);