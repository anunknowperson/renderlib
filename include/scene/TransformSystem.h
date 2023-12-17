#pragma once
#include "LocalTransformComponent.h"
#include "GlobalTransformComponent.h"
#include "flecs.h"

void TransformSystem(flecs::world &world);

void SetLocalFromMatrix(flecs::entity e, const glm::mat4& mat);

void SetLocalFromPosition(flecs::entity e, const glm::vec3& pos);

void SetLocalFromRotation(flecs::entity e, const glm::quat& rot);

void SetLocalFromScale(flecs::entity e, const glm::float64& scale);

glm::f64mat4 GetMatrixFromLocal(flecs::entity e);

glm::f64mat4 GetMatrixFromLocal(const LocalTransform& t);

void LocalRotate(flecs::entity e, const glm::f64quat& rot);

void LocalRotateX(flecs::entity e, const glm::float64& angle);

void LocalRotateY(flecs::entity e, const glm::float64& angle);

void LocalRotateZ(flecs::entity e, const glm::float64& angle);

void LocalTranslate(flecs::entity e, const glm::vec3& pos);

void LocalTranslateX(flecs::entity e, const glm::float64& distance);

void LocalTranslateY(flecs::entity e, const glm::float64& distance);

void LocalTranslateZ(flecs::entity e, const glm::float64& distance);

void LocalSetScale(flecs::entity e, const glm::float64& scale);

void SetLocalFromEntity(flecs::entity e, const flecs::entity& parent);

void InverseLocal(flecs::entity e);


void SetGlobalFromPosition(flecs::entity e, const glm::f64vec3& pos);

void SetGlobalFromRotation(flecs::entity e, const glm::f64quat& rot);

void SetGlobalFromScale(flecs::entity e, const glm::float64& scale);

void GlobalRotate(flecs::entity e, const glm::f64quat& rot);

void GlobalRotateX(flecs::entity e, const glm::float64& angle);

void GlobalRotateY(flecs::entity e, const glm::float64& angle);

void GlobalRotateZ(flecs::entity e, const glm::float64& angle);

void GlobalTranslate(flecs::entity e, const glm::f64vec3& pos);

void GlobalTranslateX(flecs::entity e, const glm::float64& distance);

void GlobalTranslateY(flecs::entity e, const glm::float64& distance);

void GlobalTranslateZ(flecs::entity e, const glm::float64& distance);

void GlobalSetScale(flecs::entity e, const glm::float64& scale);

void SetGlobalFromEntity(flecs::entity e, const flecs::entity& parent);

void InverseGlobal(flecs::entity e);