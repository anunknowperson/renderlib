#include "scene/MeshSystem.h"

namespace {

void UpdateMesh(flecs::entity e, GlobalTransform &gt) {
    engine::graphics::Graphics::getInstance()->set_mesh_instance_transform(
            e.get_mut<MeshComponent>()->MeshID, gt.TransformMatrix);
}

void DestroyMesh(MeshComponent &mc) {
    engine::graphics::Graphics::getInstance()->free_mesh_instance(mc.MeshID);
}
}  // namespace

void MeshSystem(flecs::world &world) {
    world.system<GlobalTransform>("UpdateMesh")
            .kind(flecs::OnUpdate)
            .each(UpdateMesh);
    world.system<MeshComponent>("DestroyMesh")
            .kind(flecs::OnRemove)
            .each(DestroyMesh);
}