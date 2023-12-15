#pragma once
#include "flecs.h"
#include <vector>
#include "core/logging.h"
#include <algorithm>

struct Parent {
    flecs::entity parent;
};

struct Child {
    std::vector<flecs::entity> children;
};

struct PreviousParent {
    flecs::entity parent;
};