#include "scene/mesh_system.h"
#include <iostream>

namespace {

    void CreateMesh(flecs::entity e, GlobalTransform& gt)
    {
        if (!e.has<MeshComponent>()) {
            e.set(MeshComponent{engine::graphics::Graphics::get_instance()->create_mesh_instance()});
            std::cout << "papa" << std::endl;
            std::cout << e.get_mut<MeshComponent>()->MeshID << std::endl;
        }

    }

    void UpdateMesh(flecs::entity e, GlobalTransform& gt)
    {

        engine::graphics::Graphics::get_instance()->set_mesh_instance_transform(e.get_mut<MeshComponent>()->MeshID, gt.TransformMatrix);

    }

    void DestroyMesh(flecs::entity e, MeshComponent& mc)
    {
        engine::graphics::Graphics::get_instance()->free_mesh_instance(mc.MeshID);
    }
}

void MeshSystem(flecs::world& world)
{
    world.system< GlobalTransform>("CreateMesh")
            .kind(flecs::OnUpdate)
            .each(CreateMesh);
    world.system<GlobalTransform>("UpdateMesh")
            .kind(flecs::OnUpdate)
            .each(UpdateMesh);
    world.system<MeshComponent>("DestroyMesh")
            .kind(flecs::OnRemove)
            .each(DestroyMesh);
}