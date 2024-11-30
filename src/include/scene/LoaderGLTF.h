#pragma once

#include "scene/Mesh.h"

struct LoaderGLTF {
    static std::optional<const std::shared_ptr<const Mesh::GLTF::LoadedGLTF>> loadGLTF(
            VulkanEngine& engine, std::string_view filePath);
};
