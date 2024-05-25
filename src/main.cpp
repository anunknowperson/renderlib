#include <iostream>

#include "Controller.h"
#include "Model.h"
#include "View.h"


int main(int argc, char *args[]) {

    try {
        auto const model = createModel();
        auto const controller = createController(model);
        auto const view = createView(controller, model);

        view->run();
    }
    catch (std::runtime_error const &e) {
        std::cerr << "Unhandled exception: " << e.what() << '\n';
    }

    return 0;

}