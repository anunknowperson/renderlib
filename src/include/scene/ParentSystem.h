#pragma once

#include "flecs.h"
#include "ParentComponent.h"


/** @brief Sets the parent of an entity.
 * @param child The entity to set the parent of.
 * @param parent The entity to set as the parent.
 * */
void setRelation(flecs::entity child, flecs::entity parent);

/** @brief Removes the parent of an entity.
 * @param child The entity to remove the parent of.
 * */
void removeRelation(flecs::entity child);

/** @brief Removes the child from an entity.
 * @param removing_child The child to remove.
 * @param parent The entity to remove the child from.
 * */
void removeRelation(flecs::entity removing_child, flecs::entity parent);

/** @brief Sets up the ParentSystem in the given world.
 *
 * @details The system guarantees the following invariants after each update: <br>
 * - Invalid or deleted entities that were children are removed from the parent's child lists. <br>
 * - If the parent entity is deleted or invalid, its children are deleted. <br>
 * - If an entity changes its parent, it will be removed from the list of children of the previous parent. <br>
 * - All dependent entities have exactly one parent and all parent entities have at least one child.
 * @param world The world to set up the system in.
 * */
void ParentSystem(flecs::world & world);