#pragma once

#include <memory>

#include "graphics/graphics.h"

namespace engine
{

class application {

public:


    application();
    ~application();

private:

    std::unique_ptr<graphics::graphics> graphics;

    void create_singletons();

};

}