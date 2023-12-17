#include "scene/TransformSystem.h"
#include "scene/ParentSystem.h"
#include "glm/gtx/matrix_decompose.hpp"

namespace {
    void UpdateChildrenGlobal(flecs::entity e, GlobalTransform &t)
    {
        if (e.has<Child>())
        {
            auto c = e.get<Child>();
            for (auto &child : c->children)
            {
                auto *child_transform = child.get<LocalTransform>();
                auto *child_global_transform = child.get_mut<GlobalTransform>();
                child_global_transform->TransformMatrix = t.TransformMatrix *
                                                           glm::translate(glm::f64mat4(1.0f), child_transform->position) *
                                                           glm::mat4_cast(child_transform->rotation) *
                                                           glm::scale(glm::f64mat4(1.0f), glm::f64vec3(child_transform->scale));

            }
        }
    }

    void CreateChildLocalIfParentSet(flecs::entity e, Parent &p)
    {
        if (e.has<GlobalTransform>())
        {
            auto t = e.get<GlobalTransform>();
            auto local = t->TransformMatrix * glm::inverse(p.parent.get<GlobalTransform>()->TransformMatrix) ;
            glm::f64vec3 position;
            glm::f64quat rotation;
            glm::f64vec3 scale;
            glm::f64vec3 skew;
            glm::f64vec4 perspective;
            glm::decompose(local, scale, rotation, position, skew, perspective);
            e.set(LocalTransform{position, rotation, scale.x});
        }
    }

    void UpdateChildLocalIfParentChanged(flecs::entity e, Parent &p)
    {
        auto t = e.get<GlobalTransform>();
        auto local = t->TransformMatrix * glm::inverse(p.parent.get<GlobalTransform>()->TransformMatrix) ;
        glm::f64vec3 position;
        glm::f64quat rotation;
        glm::f64vec3 scale;
        glm::f64vec3 skew;
        glm::f64vec4 perspective;
        glm::decompose(local, scale, rotation, position, skew, perspective);
        auto *transform = e.get_mut<LocalTransform>();
        transform->position = position;
        transform->rotation = rotation;
        transform->scale = scale.x;
    }

    void UpdateChildLocalIfGlobalChanged(flecs::entity e, GlobalTransform &t)
    {
        if (e.has<Parent>())
        {
            auto p = e.get<Parent>();
            auto local = t.TransformMatrix * glm::inverse(p->parent.get<GlobalTransform>()->TransformMatrix) ;
            glm::f64vec3 position;
            glm::f64quat rotation;
            glm::f64vec3 scale;
            glm::f64vec3 skew;
            glm::f64vec4 perspective;
            glm::decompose(local, scale, rotation, position, skew, perspective);
            auto *transform = e.get_mut<LocalTransform>();
            transform->position = position;
            transform->rotation = rotation;
            transform->scale = scale.x;
        }
    }

    void UpdateChildGlobalIfLocalChanged(flecs::entity e, LocalTransform &t)
    {
        auto global = e.get_mut<GlobalTransform>()->TransformMatrix = GetMatrixFromLocal(t) *
                e.get<Parent>()->parent.get<GlobalTransform>()->TransformMatrix;
    }
}

void SetLocalFromMatrix(flecs::entity e, const glm::mat4& matrix)
{
    glm::vec3 position = matrix[3];

    glm::float64 epsilon = 0.0001;
    glm::float64 scale_x = glm::length(matrix[0]);
    glm::float64 scale_y = glm::length(matrix[1]);
    glm::float64 scale_z = glm::length(matrix[2]);

    if (abs(scale_x - scale_y) > epsilon || abs(scale_x - scale_z) > epsilon || abs(scale_y - scale_z) > epsilon) {
        LOGW("Trying to convert a float4x4 to a LocalTransform, but the scale is not uniform")
    }
    glm::float64 scale = sqrt(scale_x);

    auto pos_matrix = glm::mat3(matrix);

    glm::float64 dot_product1 = glm::dot(pos_matrix[0], pos_matrix[1]);
    glm::float64 dot_product2 = glm::dot(pos_matrix[0], pos_matrix[2]);
    glm::float64 dot_product3 = glm::dot(pos_matrix[1], pos_matrix[2]);
#ifndef NDEBUG
    if (abs(dot_product1) > epsilon || abs(dot_product2) > epsilon || abs(dot_product3) > epsilon) {
        LOGW("Trying to convert a float4x4 to a LocalTransform, but the rotation is not orthogonal");
    }
#endif
    pos_matrix = glm::orthonormalize(pos_matrix);
    glm::quat rotation = glm::quat_cast(pos_matrix);

    if (e.has<LocalTransform>()) {
        auto *transform = e.get_mut<LocalTransform>();
        transform->position = position;
        transform->rotation = rotation;
        transform->scale = scale;
    } else {
        e.set<LocalTransform>({position, rotation, scale});
    }
}

void SetLocalFromPosition(flecs::entity e, const glm::vec3& pos)
{
    if (e.has<LocalTransform>()) {
        auto *transform = e.get_mut<LocalTransform>();
        transform->position = pos;
    } else {
        e.set<LocalTransform>({pos, glm::quat(1, 0, 0, 0), 1});
    }
}

void SetLocalFromRotation(flecs::entity e, const glm::quat& rot)
{
    if (e.has<LocalTransform>()) {
        auto *transform = e.get_mut<LocalTransform>();
        transform->rotation = rot;
    } else {
        e.set<LocalTransform>({glm::vec3(0, 0, 0), rot, 1});
    }
}

void SetLocalFromScale(flecs::entity e, const glm::float64& scale)
{
    if (e.has<LocalTransform>()) {
        auto *transform = e.get_mut<LocalTransform>();
        transform->scale = scale;
    } else {
        e.set<LocalTransform>({glm::vec3(0, 0, 0), glm::quat(1, 0, 0, 0), scale});
    }
}

glm::f64mat4 GetMatrixFromLocal(flecs::entity e)
{
#ifdef NDEBUG
    if (!e.has<LocalTransform>()) {
        LOGW("Trying to convert a LocalTransform to a float4x4, but the entity does not have a LocalTransform");
        return;
    }
#endif
    auto *transform = e.get_mut<LocalTransform>();
    return glm::translate(glm::f64mat4(1.0f), transform->position) *
           glm::mat4_cast(transform->rotation) *
           glm::scale(glm::f64mat4(1.0f), glm::f64vec3(transform->scale));
}

glm::f64mat4 GetMatrixFromLocal(const LocalTransform& t)
{
    return glm::translate(glm::f64mat4(1.0f), t.position) *
           glm::mat4_cast(t.rotation) *
           glm::scale(glm::f64mat4(1.0f), glm::f64vec3(t.scale));
}


void LocalRotate(flecs::entity e, const glm::f64quat& rot)
{
    if (e.has<LocalTransform>()) {
        auto *transform = e.get_mut<LocalTransform>();
        transform->rotation = rot * transform->rotation;
    }
#ifndef NDEBUG
    else {
        LOGW("Trying to rotate an entity, but it does not have a LocalTransform");
    }
#endif
}

void LocalRotateX(flecs::entity e, const glm::float64& angle)
{
    if (e.has<LocalTransform>()) {
        auto *transform = e.get_mut<LocalTransform>();
        transform->rotation = transform->rotation * glm::f64quat(angle, glm::f64vec3(1, 0, 0));
    }
#ifndef NDEBUG
    else {
        LOGW("Trying to rotate an entity, but it does not have a LocalTransform");
    }
#endif
}

void LocalRotateY(flecs::entity e, const glm::float64& angle)
{
    if (e.has<LocalTransform>()) {
        auto *transform = e.get_mut<LocalTransform>();
        transform->rotation = transform->rotation * glm::f64quat(angle, glm::f64vec3(0, 1, 0));
    }
#ifndef NDEBUG
    else {
        LOGW("Trying to rotate an entity, but it does not have a LocalTransform");
    }
#endif
}

void LocalRotateZ(flecs::entity e, const glm::float64& angle)
{
    if (e.has<LocalTransform>()) {
        auto *transform = e.get_mut<LocalTransform>();
        transform->rotation = transform->rotation * glm::f64quat(angle, glm::f64vec3(0, 0, 1));
    }
#ifndef NDEBUG
    else {
        LOGW("Trying to rotate an entity, but it does not have a LocalTransform");
    }
#endif
}

void LocalTranslate(flecs::entity e, const glm::vec3& pos)
{
    if (e.has<LocalTransform>()) {
        auto *transform = e.get_mut<LocalTransform>();
        transform->position += pos;
    }
#ifndef NDEBUG
    else {
        LOGW("Trying to translate an entity, but it does not have a LocalTransform");
    }
#endif
}

void LocalTranslateX(flecs::entity e, const glm::float64& distance)
{
    if (e.has<LocalTransform>()) {
        auto *transform = e.get_mut<LocalTransform>();
        transform->position.x += distance;
    }
#ifndef NDEBUG
    else {
        LOGW("Trying to translate an entity, but it does not have a LocalTransform");
    }
#endif
}

void LocalTranslateY(flecs::entity e, const glm::float64& distance)
{
    if (e.has<LocalTransform>()) {
        auto *transform = e.get_mut<LocalTransform>();
        transform->position.y += distance;
    }
#ifndef NDEBUG
    else {
        LOGW("Trying to translate an entity, but it does not have a LocalTransform");
    }
#endif
}

void LocalTranslateZ(flecs::entity e, const glm::float64& distance)
{
    if (e.has<LocalTransform>()) {
        auto *transform = e.get_mut<LocalTransform>();
        transform->position.z += distance;
    }
#ifndef NDEBUG
    else {
        LOGW("Trying to translate an entity, but it does not have a LocalTransform");
    }
#endif
}

void LocalSetScale(flecs::entity e, const glm::float64& scale)
{
    if (e.has<LocalTransform>()) {
        auto *transform = e.get_mut<LocalTransform>();
        transform->scale = scale;
    }
#ifndef NDEBUG
    else {
        LOGW("Trying to set the scale of an entity, but it does not have a LocalTransform");
    }
#endif
}

void SetLocalFromEntity(flecs::entity e, const flecs::entity& parent)
{
    if (e.has<LocalTransform>() && parent.has<LocalTransform>()) {
        auto *transform = e.get_mut<LocalTransform>();
        auto *parent_transform = parent.get_mut<LocalTransform>();
        transform->position = parent_transform->position;
        transform->rotation = parent_transform->rotation;
        transform->scale = parent_transform->scale;
    }
#ifndef NDEBUG
    else {
        LOGW("Trying to set the LocalTransform of an entity from another entity, but one of them does not have a LocalTransform");
    }
#endif
}

void InverseLocal(flecs::entity e)
{
    if (e.has<LocalTransform>()) {
        auto *transform = e.get_mut<LocalTransform>();
        transform->rotation = glm::inverse(transform->rotation);
        transform->position = -transform->position;
    }
#ifndef NDEBUG
    else {
        LOGW("Trying to inverse transform an entity, but it does not have a LocalTransform");
    }
#endif
}

void SetGlobalFromPosition(flecs::entity e, const glm::f64vec3& pos)
{
    e.set(GlobalTransform{glm::translate(glm::f64mat4(1.0f), pos)});
}

void SetGlobalFromRotation(flecs::entity e, const glm::f64quat& rot)
{
    e.set(GlobalTransform{glm::mat4_cast(rot)});
}

void SetGlobalFromScale(flecs::entity e, const glm::float64& scale)
{
    e.set(GlobalTransform{glm::scale(glm::f64mat4(1.0f), glm::f64vec3(scale))});
}

void GlobalRotate(flecs::entity e, const glm::f64quat& rot)
{
    e.set(GlobalTransform{glm::f64mat4(1.0f) * glm::mat4_cast(rot)});
}

void GlobalRotateX(flecs::entity e, const glm::float64& angle)
{
    e.set(GlobalTransform{glm::f64mat4(1.0f) * glm::mat4_cast(glm::f64quat(angle, glm::f64vec3(1, 0, 0)))});
}

void GlobalRotateY(flecs::entity e, const glm::float64& angle)
{
    e.set(GlobalTransform{glm::f64mat4(1.0f) * glm::mat4_cast(glm::f64quat(angle, glm::f64vec3(0, 1, 0)))});
}

void GlobalRotateZ(flecs::entity e, const glm::float64& angle)
{
    e.set(GlobalTransform{glm::f64mat4(1.0f) * glm::mat4_cast(glm::f64quat(angle, glm::f64vec3(0, 0, 1)))});
}

void GlobalTranslate(flecs::entity e, const glm::f64vec3& pos)
{
    e.set(GlobalTransform{glm::translate(glm::f64mat4(1.0f), pos)});
}

void GlobalTranslateX(flecs::entity e, const glm::float64& distance)
{
    e.set(GlobalTransform{glm::translate(glm::f64mat4(1.0f), glm::f64vec3(distance, 0, 0))});
}

void GlobalTranslateY(flecs::entity e, const glm::float64& distance)
{
    e.set(GlobalTransform{glm::translate(glm::f64mat4(1.0f), glm::f64vec3(0, distance, 0))});
}

void GlobalTranslateZ(flecs::entity e, const glm::float64& distance)
{
    e.set(GlobalTransform{glm::translate(glm::f64mat4(1.0f), glm::f64vec3(0, 0, distance))});
}

void GlobalSetScale(flecs::entity e, const glm::float64& scale)
{
    e.set(GlobalTransform{glm::scale(glm::f64mat4(1.0f), glm::f64vec3(scale))});
}

void SetGlobalFromEntity(flecs::entity e, const flecs::entity& parent)
{
    if (parent.has<GlobalTransform>()) {
        e.set(GlobalTransform{parent.get<GlobalTransform>()->TransformMatrix});
    }
}

void InverseGlobal(flecs::entity e)
{
    auto *transform = e.get_mut<GlobalTransform>();
    transform->TransformMatrix = glm::inverse(transform->TransformMatrix);
}


void TransformSystem(flecs::world &world)
{
    world.system<GlobalTransform>()
            .kind(flecs::OnSet)
            .each(UpdateChildrenGlobal);
    world.system<Parent>()
            .kind(flecs::OnAdd)
            .each(CreateChildLocalIfParentSet);
    world.system<Parent>()
            .kind(flecs::OnSet)
            .each(UpdateChildLocalIfParentChanged);
    world.system<GlobalTransform>()
            .kind(flecs::OnSet)
            .each(UpdateChildLocalIfGlobalChanged);
    world.system<LocalTransform>()
            .kind(flecs::OnSet)
            .each(UpdateChildGlobalIfLocalChanged);
}