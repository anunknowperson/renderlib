#include <iostream>

#include "core/Controller.h"
#include "core/Model.h"
#include "core/View.h"


int main([[maybe_unused]] int argc, [[maybe_unused]] char *args[]) {

    try {
        auto const model = createModel();
        auto const controller = createController(model);
        auto const view = createView(controller, model);


        view->run();
    } catch (std::runtime_error const &e) {
        std::cerr << "Unhandled exception: " << e.what() << '\n';
    }

    return 0;
}