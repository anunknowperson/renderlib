// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "Controller.h"
#include "impl/ControllerImpl.h"

IController::Ptr createController(IModel::Ptr ptr)
{
	return std::make_shared<ControllerImpl>(std::move(ptr));
}