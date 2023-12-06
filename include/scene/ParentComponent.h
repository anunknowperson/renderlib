#pragma once
#include "flecs.h"
#include <vector>

struct Parent {
    flecs::entity Parent;
};

struct Child {
    std::vector<flecs::entity> Child;
};

struct PreviousParent {
    flecs::entity Parent;
};