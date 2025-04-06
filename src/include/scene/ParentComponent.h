#pragma once

#include <algorithm>
#include <vector>

#include "core/Logging.h"
#include "flecs.h"

/** @brief Entity component containing the parent entity.
 * @see ParentSystem
 * */
struct Parent {
    flecs::entity parent;
};

/** @brief Entity component containing the children entities.
 *  @see ParentSystem
 * */
struct Child {
    std::vector<flecs::entity> children;
};

/** @brief Entity component containing the previous parent entity.
 * @see ParentSystem
 * */
struct PreviousParent {
    flecs::entity parent;
};