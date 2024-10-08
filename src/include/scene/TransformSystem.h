#pragma once

#include "LocalTransformComponent.h"
#include "GlobalTransformComponent.h"
#include "flecs.h"
#include <iostream>


/** @brief a method to set the local coordinates of an entity from a matrix.
 *
 * @param e entity into which the coordinates are set.
 * @param mat orthonormalized matrix with uniform scale to set the local coordinates.
 * */
void setLocalFromMatrix(flecs::entity e, const glm::mat4 &mat);

/** @brief a method to set the global coordinates of an entity from a position.
 *
 * @param e entity into which the coordinates are set.
 * @param pos (x, y, z) vector.
 * */
void setLocalFromPosition(flecs::entity e, const glm::vec3 &pos);

/** @brief a method to set the global coordinates of an entity from a rotation.
 *
 * @param e entity into which the coordinates are set.
 * @param rot quaternion.
 * */
void setLocalFromRotation(flecs::entity e, const glm::quat &rot);

/** @brief a method to set the global coordinates of an entity from a scale.
 *
 * @param e entity into which the coordinates are set.
 * @param scale uniform scale.
 * */
void setLocalFromScale(flecs::entity e, const glm::float64 &scale);

/** @brief a method to get the local coordinates of an entity as a matrix.
 *
 * @param e entity from which the coordinates are retrieved.
 * @return orthonormalized matrix with uniform scale.
 * */
glm::f64mat4 getMatrixFromLocal(flecs::entity e);

glm::f64mat4 getMatrixFromLocal(const LocalTransform &t);

/** @brief a method to rotate an entity locally.
 *
 * @param e entity to rotate.
 * @param rot quaternion.
 * */
void localRotate(flecs::entity e, const glm::f64quat &rot);

/** @brief a method to rotate an entity locally around the x axis.
 *
 * @param e entity to rotate.
 * @param angle angle in radians.
 * */
void localRotateX(flecs::entity e, const glm::float64 &angle);

/** @brief a method to rotate an entity locally around the y axis.
 *
 * @param e entity to rotate.
 * @param angle angle in radians.
 * */
void localRotateY(flecs::entity e, const glm::float64 &angle);

/** @brief a method to rotate an entity locally around the z axis.
 *
 * @param e entity to rotate.
 * @param angle angle in radians.
 * */
void localRotateZ(flecs::entity e, const glm::float64 &angle);

/** @brief a method to translate an entity locally.
 *
 * @param e entity to translate.
 * @param pos (x, y, z) vector.
 * */
void localTranslate(flecs::entity e, const glm::vec3 &pos);

/** @brief a method to translate an entity locally along the x axis.
 *
 * @param e entity to translate.
 * @param distance distance to translate.
 * */
void localTranslateX(flecs::entity e, const glm::float64 &distance);

/** @brief a method to translate an entity locally along the y axis.
 *
 * @param e entity to translate.
 * @param distance distance to translate.
 * */
void localTranslateY(flecs::entity e, const glm::float64 &distance);

/** @brief a method to translate an entity locally along the z axis.
 *
 * @param e entity to translate.
 * @param distance distance to translate.
 * */
void localTranslateZ(flecs::entity e, const glm::float64 &distance);

/** @brief a method to set the scale of an entity locally.
 *
 * @param e entity to scale.
 * @param scale uniform scale.
 * */
void localSetScale(flecs::entity e, const glm::float64 &scale);

/** @brief a method to set the local coordinates of an entity from another entity.
 *
 * @param e entity into which the coordinates are set.
 * @param parent entity from which the coordinates are retrieved.
 * */
void setLocalFromEntity(flecs::entity e, const flecs::entity &parent);

/** @brief a method to inverse local matrix.
 *
 * @param e entity whose coordinates are to be inverted
 * */
void inverseLocal(flecs::entity e);

/** @brief a method to set the global coordinates of an entity from position.
 * @param e entity into which the coordinates are set.
 * @param pos (x, y, z) vector.
 * */
void setGlobalFromPosition(flecs::entity e, const glm::f64vec3 &pos);

/** @brief a method to set the global coordinates of an entity from a rotation.
 *
 * @param e entity into which the coordinates are set.
 * @param rot quaternion.
 * */
void setGlobalFromRotation(flecs::entity e, const glm::f64quat &rot);

/** @brief a method to set the global coordinates of an entity from a scale.
 *
 * @param e entity into which the coordinates are set.
 * @param scale uniform scale.
 * */
void setGlobalFromScale(flecs::entity e, const glm::float64 &scale);

/** @brief a method to rotate an entity globally.
 *
 * @param e entity to rotate.
 * @param rot quaternion.
 * */
void globalRotate(flecs::entity e, const glm::f64quat &rot);

/** @brief a method to rotate an entity globally around the x axis.
 *
 * @param e entity to rotate.
 * @param angle angle in radians.
 * */
void globalRotateX(flecs::entity e, const glm::float64 &angle);

/** @brief a method to rotate an entity globally around the y axis.
 *
 * @param e entity to rotate.
 * @param angle angle in radians.
 * */
void globalRotateY(flecs::entity e, const glm::float64 &angle);

/** @brief a method to rotate an entity globally around the z axis.
 *
 * @param e entity to rotate.
 * @param angle angle in radians.
 * */
void globalRotateZ(flecs::entity e, const glm::float64 &angle);

/** @brief a method to translate an entity globally.
 *
 * @param e entity to translate.
 * @param pos (x, y, z) vector.
 * */
void globalTranslate(flecs::entity e, const glm::f64vec3 &pos);

/** @brief a method to translate an entity globally along the x axis.
 *
 * @param e entity to translate.
 * @param distance distance to translate.
 * */
void globalTranslateX(flecs::entity e, const glm::float64 &distance);

/** @brief a method to translate an entity globally along the y axis.
 *
 * @param e entity to translate.
 * @param distance distance to translate.
 * */
void globalTranslateY(flecs::entity e, const glm::float64 &distance);

/** @brief a method to translate an entity globally along the z axis.
 *
 * @param e entity to translate.
 * @param distance distance to translate.
 * */
void globalTranslateZ(flecs::entity e, const glm::float64 &distance);

/** @brief a method to set the scale of an entity globally.
 *
 * @param e entity to scale.
 * @param scale uniform scale.
 * */
void globalSetScale(flecs::entity e, const glm::float64 &scale);

/** @brief a method to set the global coordinates of an entity from another entity.
 *
 * @param e entity into which the coordinates are set.
 * @param parent entity from which the coordinates are retrieved.
 * */
void setGlobalFromEntity(flecs::entity e, const flecs::entity &parent);

/** @brief a method to inverse global matrix.
 *
 * @param e entity whose coordinates are to be inverted
 * */
void inverseGlobal(flecs::entity e);

/**
 * @brief Sets up the TransformSystem in the given world.
 *
 * @details
 *
 * The system guarantees the following invariants after each update: <br>
 * - Every time the parent's coordinates are changed, the children's coordinates also changes. <br>
 * - When children change global coordinates, local coordinates change and vice versa. <br>
 * - Children will always have local coordinates. <br>
 *
 * @param world The world to set up the system in.
 *
 * @see LocalTransformComponent, GlobalTransformComponent, ParentSystem.
 * */
void TransformSystem(flecs::world & world);