#pragma once
#include "flecs.h"
#include "scene/ParentComponent.h"


void setRelation(flecs::entity child, flecs::entity parent);

void removeRelation(flecs::entity child);

void removeRelation(flecs::entity removing_child, flecs::entity parent);

void HierarchySystem(flecs::world& world);