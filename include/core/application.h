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
    void create_singletons();

};

}