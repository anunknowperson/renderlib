#include "graphics/graphics.h"


namespace engine::graphics{

Graphics* Graphics::singleton = nullptr;

Graphics::Graphics() {

    singleton = this;

}

Graphics::~Graphics() {

}

Graphics *Graphics::get_instance() {
    return singleton;
}

}