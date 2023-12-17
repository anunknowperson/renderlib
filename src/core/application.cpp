#include "core/application.h"

namespace engine
{


    application::application() {

        create_singletons();
    }

    void application::create_singletons() {
        graphics = std::make_unique<graphics::graphics>();


    }

    application::~application() {

    }
}