#include "core/View.h"

#include <memory>
#include <utility>

#include "core/ViewImpl.h"

IView::Ptr createView() {
    return std::make_unique<ViewImpl>();
}