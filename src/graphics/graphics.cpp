#include "graphics/graphics.h"


namespace engine::graphics{

graphics::graphics() {

    singleton = this;

}

graphics *graphics::get_instance() {
    return singleton;
}

}