#include "core/View.h"
#include "core/ViewImpl.h"

IView::Ptr createView(IController::Ptr c_ptr, IModel::Ptr m_ptr) {
    return std::make_unique<ViewImpl>(std::move(c_ptr), std::move(m_ptr));
}