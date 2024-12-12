#include "scene/MeshSystem.h"

namespace {

void updateMesh(flecs::entity e, GlobalTransform &gt) {
    engine::graphics::Graphics::getInstance()->setMeshInstanceTransform(
            e.get_mut<MeshComponent>()->MeshID, gt.transform_matrix);
}

void destroyMesh(MeshComponent &mc) {
    engine::graphics::Graphics::getInstance()->freeMeshInstance(mc.MeshID);
}
}  // namespace

void meshSystem(flecs::world &world) {
    world.system<GlobalTransform>("UpdateMesh")
            .kind(flecs::OnUpdate)
            .each(updateMesh);
    world.system<MeshComponent>("DestroyMesh")
            .kind(flecs::OnRemove)
            .each(destroyMesh);
}