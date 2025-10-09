#include "graphics/vulkan/RenderableGLTF.h"
#include "graphics/vulkan/vk_loader.h"

IRenderable::Ptr createRenderableGLTF(RenderableGLTF::LoadedGltfPtr ptr)
{
    return std::make_shared<RenderableGLTF>(std::move(ptr));
}