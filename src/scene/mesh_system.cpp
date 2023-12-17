#include "scene/mesh_system.h"

namespace {
    auto Graphics = engine::graphics::Graphics::get_instance();

    void CreateMesh(flecs::entity e, GlobalTransform& gt)
    {
        e.set(MeshComponent{Graphics->create_mesh_instance()});
    }

    void UpdateMesh(flecs::entity e, GlobalTransform& gt)
    {
        Graphics->set_mesh_instance_transform(e.get<MeshComponent>()->MeshID, gt.TransformMatrix);
    }

    void DestroyMesh(flecs::entity e, MeshComponent& mc)
    {
        Graphics->free_mesh_instance(mc.MeshID);
    }
}

void MeshSystem(flecs::world& world)
{
    world.system< GlobalTransform>()
            .kind(flecs::OnAdd)
            .each(CreateMesh);
    world.system<GlobalTransform>()
            .kind(flecs::OnSet)
            .each(UpdateMesh);
    world.system<MeshComponent>()
            .kind(flecs::OnRemove)
            .each(DestroyMesh);
}