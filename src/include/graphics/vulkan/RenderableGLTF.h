#pragma once

#include "graphics/vulkan/vk_types.h"
#include "graphics/vulkan/vk_loader.h"
#include "scene/Mesh.h"

IRenderable::Ptr createRenderableGLTF(RenderableGLTF::LoadedGltfPtr ptr);