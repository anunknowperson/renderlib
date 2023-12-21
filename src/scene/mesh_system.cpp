#include "scene/mesh_system.h"
#include <iostream>

namespace {

    void CreateMesh(flecs::entity e, GlobalTransform& gt)
    {
        if (!e.has<MeshComponent>()) {
            auto temp = engine::graphics::Graphics::getInstance()->create_mesh_instance();
            e.set(MeshComponent{temp});
            std::cout << e.get<GlobalTransform>()->TransformMatrix[0][0] << std::endl;
            std::cout << e.has<MeshComponent>() << ", " << e.has<GlobalTransform>() << std::endl;
            std::cout << "create mesh" << std::endl;
            std::cout << temp << std::endl;
            auto res = e.get<MeshComponent>()->MeshID;
            std::cout << res << std::endl;
        }

    }

    void UpdateMesh(flecs::entity e, GlobalTransform& gt)
    {
        engine::graphics::Graphics::getInstance()->set_mesh_instance_transform(e.get_mut<MeshComponent>()->MeshID, gt.TransformMatrix);

    }

    void DestroyMesh(flecs::entity e, MeshComponent& mc)
    {
        engine::graphics::Graphics::getInstance()->free_mesh_instance(mc.MeshID);
    }
}

void MeshSystem(flecs::world& world)
{
    std::cout << "MeshSystem is being called" << std::endl;

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