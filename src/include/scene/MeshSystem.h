#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "MeshComponent.h"
#include "GlobalTransformComponent.h"
#include "graphics/Graphics.h"


/**
 * @brief Sets up the MeshSystem in the given world.
 *
 * @details
 * The MeshSystem is responsible for updating the MeshComponent of entities that have a GlobalTransform
 *
 * The system guarantees the following invariants after each update: <br>
 * - Every time the GlobalTransform of entity changes, the MeshComponent also changes. <br>
 * - If the entity no longer has a GlobalTransform, the MeshComponent is deleted.
 *
 * @param world The world to set up the system in.
 *
 * @see MeshComponent.
 * */
void MeshSystem(flecs::world& world);