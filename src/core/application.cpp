#include "core/application.h"

namespace engine
{


    Application::Application() {

        create_singletons();
    }

    void Application::create_singletons() {
        graphics = std::make_unique<graphics::Graphics>();


    }

    Application::~Application() {

    }
}