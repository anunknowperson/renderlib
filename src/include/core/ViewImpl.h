#pragma once

#include "interfaces/IView.h"

class ViewImpl : public IView {
public:
    ViewImpl();
    void render() override;
};
