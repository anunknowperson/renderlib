#include "graphics/vulkan/vk_loader.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/util.hpp>
#include <fmt/base.h>
#include <vk_mem_alloc.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/detail/qualifier.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec3.hpp>
#include <glm/detail/type_vec4.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <span>
#include <utility>
#include <variant>

#include "graphics/vulkan/vk_descriptors.h"
#include "graphics/vulkan/vk_engine.h"
#include "graphics/vulkan/vk_types.h"

std::optional<std::vector<std::shared_ptr<MeshAsset>>> loadGltfMeshes(
        VulkanEngine* engine, const std::filesystem::path& file_path) {
    if (!std::filesystem::exists(file_path)) {
        std::cout << "Failed to find " << file_path << '\n';
        return {};
    }

    std::cout << "Loading " << file_path << '\n';

    fastgltf::Asset gltf;

    // Parse the glTF file and get the constructed asset

    static constexpr auto supportedExtensions =
            fastgltf::Extensions::KHR_mesh_quantization |
            fastgltf::Extensions::KHR_texture_transform |
            fastgltf::Extensions::KHR_materials_variants;

    fastgltf::Parser parser(supportedExtensions);

    auto path = std::filesystem::path{file_path};

    constexpr auto gltfOptions =
            fastgltf::Options::DontRequireValidAssetMember |
            fastgltf::Options::AllowDouble | fastgltf::Options::LoadGLBBuffers |
            fastgltf::Options::LoadExternalBuffers |
            fastgltf::Options::LoadExternalImages |
            fastgltf::Options::GenerateMeshIndices;

    fastgltf::GltfDataBuffer data;
    data.loadFromFile(path);

    auto asset = parser.loadGltf(&data, path.parent_path(), gltfOptions);

    if (asset) {
        gltf = std::move(asset.get());
    } else {
        fmt::print("Failed to load glTF: {} \n",
                   fastgltf::to_underlying(asset.error()));
        return {};
    }

    std::vector<std::shared_ptr<MeshAsset>> meshes;

    // use the same vectors for all meshes so that the memory doesnt reallocate
    // as often
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;
    for (auto& [primitives, _, name] : gltf.meshes) {
        MeshAsset new_mesh;

        new_mesh.name = name;

        // clear the mesh arrays each mesh, we dont want to merge them by error
        indices.clear();
        vertices.clear();

        for (auto&& p : primitives) {
            GeoSurface new_surface;
            new_surface.start_index = (uint32_t)indices.size();
            new_surface.count =
                    (uint32_t)gltf.accessors[p.indicesAccessor.value()].count;

            auto initial_vtx = static_cast<uint32_t>(vertices.size());

            // load indexes
            {
                fastgltf::Accessor& index_accessor =
                        gltf.accessors[p.indicesAccessor.value()];
                indices.reserve(indices.size() + index_accessor.count);

                fastgltf::iterateAccessor<std::uint32_t>(
                        gltf, index_accessor, [&](std::uint32_t idx) {
                            indices.push_back(idx + initial_vtx);
                        });
            }

            // load vertex positions
            {
                fastgltf::Accessor& pos_accessor =
                        gltf.accessors[p.findAttribute("POSITION")->second];
                vertices.resize(vertices.size() + pos_accessor.count);

                fastgltf::iterateAccessorWithIndex<glm::vec3>(
                        gltf, pos_accessor, [&](glm::vec3 v, size_t index) {
                            Vertex new_vtx;
                            new_vtx.position = v;
                            new_vtx.normal = {1, 0, 0};
                            new_vtx.color = glm::vec4{1.f};
                            new_vtx.uv_x = 0;
                            new_vtx.uv_y = 0;
                            vertices[initial_vtx + index] = new_vtx;
                        });
            }

            // load vertex normals
            auto normals = p.findAttribute("NORMAL");
            if (normals != p.attributes.end()) {
                fastgltf::iterateAccessorWithIndex<glm::vec3>(
                        gltf, gltf.accessors[normals->second],
                        [&](glm::vec3 v, size_t index) {
                            vertices[initial_vtx + index].normal = v;
                        });
            }

            // load UVs
            auto uv = p.findAttribute("TEXCOORD_0");
            if (uv != p.attributes.end()) {
                fastgltf::iterateAccessorWithIndex<glm::vec2>(
                        gltf, gltf.accessors[uv->second],
                        [&](glm::vec2 v, size_t index) {
                            vertices[initial_vtx + index].uv_x = v.x;
                            vertices[initial_vtx + index].uv_y = v.y;
                        });
            }

            // load vertex colors
            auto colors = p.findAttribute("COLOR_0");
            if (colors != p.attributes.end()) {
                fastgltf::iterateAccessorWithIndex<glm::vec4>(
                        gltf, gltf.accessors[colors->second],
                        [&](glm::vec4 v, size_t index) {
                            vertices[initial_vtx + index].color = v;
                        });
            }
            new_mesh.surfaces.push_back(new_surface);
        }

        // display the vertex normals
        constexpr bool OverrideColors = true;
        if (OverrideColors) {
            for (Vertex& vtx : vertices) {
                vtx.color = glm::vec4(vtx.normal, 1.f);
            }
        }
        new_mesh.mesh_buffers = engine->uploadMesh(indices, vertices);

        meshes.emplace_back(std::make_shared<MeshAsset>(std::move(new_mesh)));
    }

    return meshes;
}

VkFilter extractFilter(fastgltf::Filter filter) {
    switch (filter) {
            // nearest samplers
        case fastgltf::Filter::Nearest:
        case fastgltf::Filter::NearestMipMapNearest:
        case fastgltf::Filter::NearestMipMapLinear:
            return VK_FILTER_NEAREST;

            // linear samplers
        case fastgltf::Filter::Linear:
        case fastgltf::Filter::LinearMipMapNearest:
        case fastgltf::Filter::LinearMipMapLinear:
        default:
            return VK_FILTER_LINEAR;
    }
}

VkSamplerMipmapMode extractMipmapMode(fastgltf::Filter filter) {
    switch (filter) {
        case fastgltf::Filter::NearestMipMapNearest:
        case fastgltf::Filter::LinearMipMapNearest:
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;

        case fastgltf::Filter::NearestMipMapLinear:
        case fastgltf::Filter::LinearMipMapLinear:
        default:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}

std::optional<std::shared_ptr<LoadedGLTF>> loadGltf(VulkanEngine* engine,
                                                    std::string_view file_path) {
    fmt::print("Loading GLTF: {}", file_path);

    auto scene = std::make_shared<LoadedGLTF>();
    scene->creator = engine;
    LoadedGLTF& file = *scene;

    fastgltf::Parser parser{};

    constexpr auto gltfOptions =
            fastgltf::Options::DontRequireValidAssetMember |
            fastgltf::Options::AllowDouble | fastgltf::Options::LoadGLBBuffers |
            fastgltf::Options::LoadExternalBuffers;
    // fastgltf::Options::LoadExternalImages;

    fastgltf::GltfDataBuffer data;
    data.loadFromFile(file_path);

    fastgltf::Asset gltf;

    std::filesystem::path path = file_path;

    auto type = fastgltf::determineGltfFileType(&data);
    if (type == fastgltf::GltfType::glTF) {
        auto load = parser.loadGltf(&data, path.parent_path(), gltfOptions);
        if (load) {
            gltf = std::move(load.get());
        } else {
            std::cerr << "Failed to load glTF: "
                      << fastgltf::to_underlying(load.error()) << std::endl;
            return {};
        }
    } else if (type == fastgltf::GltfType::GLB) {
        auto load =
                parser.loadGltfBinary(&data, path.parent_path(), gltfOptions);
        if (load) {
            gltf = std::move(load.get());
        } else {
            std::cerr << "Failed to load glTF: "
                      << fastgltf::to_underlying(load.error()) << std::endl;
            return {};
        }
    } else {
        std::cerr << "Failed to determine glTF container" << std::endl;
        return {};
    }

    // we can stimate the descriptors we will need accurately
    std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> sizes = {
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1}};

    file.descriptor_pool.init(engine->device,
                             static_cast<uint32_t>(gltf.materials.size()),
                             sizes);

    // load samplers
    for (fastgltf::Sampler& sampler : gltf.samplers) {
        VkSamplerCreateInfo sampl = {
                .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .pNext = nullptr};
        sampl.maxLod = VK_LOD_CLAMP_NONE;
        sampl.minLod = 0;

        sampl.magFilter = extractFilter(
                sampler.magFilter.value_or(fastgltf::Filter::Nearest));
        sampl.minFilter = extractFilter(
                sampler.minFilter.value_or(fastgltf::Filter::Nearest));

        sampl.mipmapMode = extractMipmapMode(
                sampler.minFilter.value_or(fastgltf::Filter::Nearest));

        VkSampler newSampler;
        vkCreateSampler(engine->device, &sampl, nullptr, &newSampler);

        file.samplers.push_back(newSampler);
    }

    std::vector<std::shared_ptr<MeshAsset>> meshes;
    std::vector<std::shared_ptr<ENode>> nodes;
    std::vector<AllocatedImage> images;
    std::vector<std::shared_ptr<GLTFMaterial>> materials;

    // load all textures
    images.reserve(gltf.images.size());
    for (size_t i = 0; i < gltf.images.size(); i++) {
        images.push_back(engine->error_checkerboard_image);
    }

    // create buffer to hold the material data
    file.material_data_buffer = engine->createBuffer(
            sizeof(GLTFMetallicRoughness::MaterialConstants) *
                    gltf.materials.size(),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    uint32_t data_index = 0;
    auto* scene_material_constants =
            (GLTFMetallicRoughness::MaterialConstants*)
                    file.material_data_buffer.info.pMappedData;

    for (fastgltf::Material& mat : gltf.materials) {
        auto new_mat = std::make_shared<GLTFMaterial>();
        materials.push_back(new_mat);
        file.materials[mat.name.c_str()] = new_mat;

        GLTFMetallicRoughness::MaterialConstants constants;
        constants.color_factors.x = mat.pbrData.baseColorFactor[0];
        constants.color_factors.y = mat.pbrData.baseColorFactor[1];
        constants.color_factors.z = mat.pbrData.baseColorFactor[2];
        constants.color_factors.w = mat.pbrData.baseColorFactor[3];

        constants.metal_rough_factors.x = mat.pbrData.metallicFactor;
        constants.metal_rough_factors.y = mat.pbrData.roughnessFactor;
        // write material parameters to buffer
        scene_material_constants[data_index] = constants;

        auto pass_type = MaterialPass::MainColor;
        if (mat.alphaMode == fastgltf::AlphaMode::Blend) {
            pass_type = MaterialPass::Transparent;
        }

        GLTFMetallicRoughness::MaterialResources material_resources;
        // default the material textures
        material_resources.color_image = engine->white_image;
        material_resources.color_sampler = engine->default_sampler_linear;
        material_resources.metal_rough_image = engine->white_image;
        material_resources.metal_rough_sampler = engine->default_sampler_linear;

        // set the uniform buffer for the material data
        material_resources.data_buffer = file.material_data_buffer.buffer;
        material_resources.data_buffer_offset =
                data_index * sizeof(GLTFMetallicRoughness::MaterialConstants);
        // grab textures from gltf file
        if (mat.pbrData.baseColorTexture.has_value()) {
            size_t img = gltf.textures[mat.pbrData.baseColorTexture.value()
                                               .textureIndex]
                                 .imageIndex.value();
            size_t sampler = gltf.textures[mat.pbrData.baseColorTexture.value()
                                                   .textureIndex]
                                     .samplerIndex.value();

            material_resources.color_image = images[img];
            material_resources.color_sampler = file.samplers[sampler];
        }
        // build material
        new_mat->data = engine->metal_rough_material.writeMaterial(
                engine->device, pass_type, material_resources,
                file.descriptor_pool);

        data_index++;
    }

    // use the same vectors for all meshes so that the memory doesnt reallocate
    // as
    // often
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;

    for (auto& [primitives, _, name] : gltf.meshes) {
        auto new_mesh = std::make_shared<MeshAsset>();
        meshes.push_back(new_mesh);
        file.meshes[name.c_str()] = new_mesh;
        new_mesh->name = name;

        // clear the mesh arrays each mesh, we dont want to merge them by error
        indices.clear();
        vertices.clear();

        for (auto&& p : primitives) {
            GeoSurface new_surface;
            new_surface.start_index = (uint32_t)indices.size();
            new_surface.count =
                    (uint32_t)gltf.accessors[p.indicesAccessor.value()].count;

            auto initial_vtx = static_cast<uint32_t>(vertices.size());

            // load indexes
            {
                fastgltf::Accessor& index_accessor =
                        gltf.accessors[p.indicesAccessor.value()];
                indices.reserve(indices.size() + index_accessor.count);

                fastgltf::iterateAccessor<std::uint32_t>(
                        gltf, index_accessor, [&](std::uint32_t idx) {
                            indices.push_back(idx + initial_vtx);
                        });
            }

            // load vertex positions
            {
                fastgltf::Accessor& pos_accessor =
                        gltf.accessors[p.findAttribute("POSITION")->second];
                vertices.resize(vertices.size() + pos_accessor.count);

                fastgltf::iterateAccessorWithIndex<glm::vec3>(
                        gltf, pos_accessor, [&](glm::vec3 v, size_t index) {
                            Vertex new_vtx;
                            new_vtx.position = v;
                            new_vtx.normal = {1, 0, 0};
                            new_vtx.color = glm::vec4{1.f};
                            new_vtx.uv_x = 0;
                            new_vtx.uv_y = 0;
                            vertices[initial_vtx + index] = new_vtx;
                        });
            }

            // load vertex normals
            auto normals = p.findAttribute("NORMAL");
            if (normals != p.attributes.end()) {
                fastgltf::iterateAccessorWithIndex<glm::vec3>(
                        gltf, gltf.accessors[normals->second],
                        [&](glm::vec3 v, size_t index) {
                            vertices[initial_vtx + index].normal = v;
                        });
            }

            // load UVs
            auto uv = p.findAttribute("TEXCOORD_0");
            if (uv != p.attributes.end()) {
                fastgltf::iterateAccessorWithIndex<glm::vec2>(
                        gltf, gltf.accessors[uv->second],
                        [&](glm::vec2 v, size_t index) {
                            vertices[initial_vtx + index].uv_x = v.x;
                            vertices[initial_vtx + index].uv_y = v.y;
                        });
            }

            // load vertex colors
            auto colors = p.findAttribute("COLOR_0");
            if (colors != p.attributes.end()) {
                fastgltf::iterateAccessorWithIndex<glm::vec4>(
                        gltf, gltf.accessors[colors->second],
                        [&](glm::vec4 v, size_t index) {
                            vertices[initial_vtx + index].color = v;
                        });
            }

            if (p.materialIndex.has_value()) {
                new_surface.material = materials[p.materialIndex.value()];
            } else {
                new_surface.material = materials[0];
            }

            new_mesh->surfaces.push_back(new_surface);
        }

        new_mesh->mesh_buffers = engine->uploadMesh(indices, vertices);
    }

    // load all nodes and their meshes
    for (fastgltf::Node& node : gltf.nodes) {
        std::shared_ptr<ENode> new_node;

        // find if the node has a mesh, and if it does hook it to the mesh
        // pointer and allocate it with the meshnode class
        if (node.meshIndex.has_value()) {
            new_node = std::make_shared<MeshNode>();
            dynamic_cast<MeshNode*>(new_node.get())->mesh =
                    meshes[*node.meshIndex];
        } else {
            new_node = std::make_shared<ENode>();
        }

        nodes.push_back(new_node);
        file.nodes[node.name.c_str()];

        std::visit(fastgltf::visitor{
                           [&](const fastgltf::Node::TransformMatrix& matrix) {
                               memcpy(&new_node->local_transform, matrix.data(),
                                      sizeof(matrix));
                           },
                           [&](const fastgltf::TRS& transform) {
                               const glm::vec3 tl(transform.translation[0],
                                                  transform.translation[1],
                                                  transform.translation[2]);
                               const glm::quat rot(transform.rotation[3],
                                                   transform.rotation[0],
                                                   transform.rotation[1],
                                                   transform.rotation[2]);
                               const glm::vec3 sc(transform.scale[0],
                                                  transform.scale[1],
                                                  transform.scale[2]);

                               const glm::mat4 tm =
                                       glm::translate(glm::mat4(1.f), tl);
                               const glm::mat4 rm = glm::toMat4(rot);
                               const glm::mat4 sm =
                                       glm::scale(glm::mat4(1.f), sc);

                               new_node->local_transform = tm * rm * sm;
                           }},
                   node.transform);
    }

    // run loop again to setup transform hierarchy
    for (size_t i = 0; i < gltf.nodes.size(); i++) {
        fastgltf::Node& node = gltf.nodes[i];
        std::shared_ptr<ENode>& scene_node = nodes[i];

        for (auto& c : node.children) {
            scene_node->children.push_back(nodes[c]);
            nodes[c]->parent = scene_node;
        }
    }

    // find the top nodes, with no parents
    for (auto& node : nodes) {
        if (node->parent.lock() == nullptr) {
            file.top_nodes.push_back(node);
            node->refreshTransform(glm::mat4{1.f});
        }
    }
    return scene;
}

void LoadedGLTF::draw(const glm::mat4& top_matrix, DrawContext& ctx) {
    // create renderables from the scenenodes
    for (const auto& n : top_nodes) {
        n->draw(top_matrix, ctx);
    }
}

void LoadedGLTF::clearAll() {}
