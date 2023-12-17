#pragma once

#include <memory>

#include "graphics/graphics.h"

namespace engine
{

class Application {

public:


    Application();
    ~Application();

private:

    std::unique_ptr<graphics::Graphics> graphics;

    void create_singletons();

};

}