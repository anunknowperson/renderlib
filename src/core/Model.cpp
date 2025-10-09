#include <memory>
#include "core/ModelImpl.h"
#include "interfaces/IModel.h"

IModel::Ptr createModel() {
    return std::make_shared<ModelImpl>();
}
