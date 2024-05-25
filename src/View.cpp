// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "View.h"
#include "impl/ViewImpl.h"

IView::Ptr createView(IController::Ptr c_ptr, IModel::Ptr m_ptr)
{
	return std::make_unique<ViewImpl>(std::move(c_ptr), std::move(m_ptr));
}