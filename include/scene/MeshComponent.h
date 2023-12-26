#pragma once
#include <cstdint>


/**
 * @brief A component that stores a mesh instance ID.
 *
 * @details
 * This component is used to store the ID of a mesh instance in the graphics engine.
 *
 * @see engine::graphics::Graphics
 * */
struct MeshComponent {
    uint64_t MeshID;
};