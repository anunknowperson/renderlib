#include "scene/MeshSystem.h"

namespace {

void UpdateMesh(flecs::entity e, const GlobalTransform &gt) {
    engine::graphics::Graphics::getInstance()->setMeshInstanceTransform(
            e.get_mut<MeshComponent>()->mesh_id, gt.transform_matrix);
}

void DestroyMesh(const MeshComponent &mc) {
    engine::graphics::Graphics::getInstance()->freeMeshInstance(mc.mesh_id);
}
}  // namespace

void MeshSystem(const flecs::world &world) {
    world.system<GlobalTransform>("UpdateMesh")
            .kind(flecs::OnUpdate)
            .each(UpdateMesh);
    world.system<MeshComponent>("DestroyMesh")
            .kind(flecs::OnRemove)
            .each(DestroyMesh);
}