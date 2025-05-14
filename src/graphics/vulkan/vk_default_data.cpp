#include "graphics/vulkan/vk_default_data.h"

#include <array>
#include <filesystem>
#include <string>
#include <cassert>

#include "core/config.h"
#include "graphics/vulkan/vk_engine.h"
#include "graphics/vulkan/vk_loader.h"
#include "graphics/vulkan/vk_types.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

DefaultDataManager::DefaultDataManager(VulkanEngine* engine) : _engine(engine) {
    assert(engine != nullptr);
}

void DefaultDataManager::init_default_data() {
    std::array<Vertex, 4> rect_vertices{};

    rect_vertices[0].position = {0.5, -0.5, 0};
    rect_vertices[1].position = {0.5, 0.5, 0};
    rect_vertices[2].position = {-0.5, -0.5, 0};
    rect_vertices[3].position = {-0.5, 0.5, 0};

    rect_vertices[0].color = {0, 0, 0, 1};
    rect_vertices[1].color = {0.5, 0.5, 0.5, 1};
    rect_vertices[2].color = {1, 0, 0, 1};
    rect_vertices[3].color = {0, 1, 0, 1};

    std::array<uint32_t, 6> rect_indices{};

    rect_indices[0] = 0;
    rect_indices[1] = 1;
    rect_indices[2] = 2;

    const auto path_to_assets = std::string(ASSETS_DIR) + "/basicmesh.glb";
    _engine->testMeshes = loadGltfMeshes(_engine, path_to_assets).value();

    // 3 default textures, white, grey, black. 1 pixel each
    const uint32_t white = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
    _engine->_whiteImage =
            _engine->create_image(&white, VkExtent3D{1, 1, 1}, VK_FORMAT_R8G8B8A8_UNORM,
                         VK_IMAGE_USAGE_SAMPLED_BIT);

    const uint32_t grey = glm::packUnorm4x8(glm::vec4(0.66f, 0.66f, 0.66f, 1));
    _engine->_greyImage =
            _engine->create_image(&grey, VkExtent3D{1, 1, 1}, VK_FORMAT_R8G8B8A8_UNORM,
                         VK_IMAGE_USAGE_SAMPLED_BIT);

    const uint32_t black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 0));
    _engine->_blackImage =
            _engine->create_image(&black, VkExtent3D{1, 1, 1}, VK_FORMAT_R8G8B8A8_UNORM,
                         VK_IMAGE_USAGE_SAMPLED_BIT);

    // checkerboard image
    const uint32_t magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
    std::array<uint32_t, 16 * 16> pixels{};  // for 16x16 checkerboard texture
    for (size_t x = 0; x < 16; x++) {
        for (size_t y = 0; y < 16; y++) {
            pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
        }
    }
    _engine->_errorCheckerboardImage =
            _engine->create_image(pixels.data(), VkExtent3D{16, 16, 1},
                         VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

    VkSamplerCreateInfo sampl = {.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};

    sampl.magFilter = VK_FILTER_NEAREST;
    sampl.minFilter = VK_FILTER_NEAREST;

    vkCreateSampler(_engine->_device, &sampl, nullptr, &_engine->_defaultSamplerNearest);

    sampl.magFilter = VK_FILTER_LINEAR;
    sampl.minFilter = VK_FILTER_LINEAR;
    vkCreateSampler(_engine->_device, &sampl, nullptr, &_engine->_defaultSamplerLinear);

    GLTFMetallic_Roughness::MaterialResources materialResources{};
    // default the material textures
    materialResources.colorImage = _engine->_whiteImage;
    materialResources.colorSampler = _engine->_defaultSamplerLinear;
    materialResources.metalRoughImage = _engine->_whiteImage;
    materialResources.metalRoughSampler = _engine->_defaultSamplerLinear;

    // set the uniform buffer for the material data
    const AllocatedBuffer materialConstants = _engine->create_buffer(
            sizeof(GLTFMetallic_Roughness::MaterialConstants),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

    void* mappedData;
    vmaMapMemory(_engine->_allocator, materialConstants.allocation, &mappedData);
    
    GLTFMetallic_Roughness::MaterialConstants* sceneUniformData = 
        static_cast<GLTFMetallic_Roughness::MaterialConstants*>(mappedData);
    
    sceneUniformData->colorFactors = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
    sceneUniformData->metal_rough_factors = glm::vec4{1.0f, 0.5f, 0.0f, 0.0f};

    vmaUnmapMemory(_engine->_allocator, materialConstants.allocation);

    _engine->_mainDeletionQueue.push_function(
            [=, this] { _engine->destroy_buffer(materialConstants); });

    materialResources.dataBuffer = materialConstants.buffer;
    materialResources.dataBufferOffset = 0;

    _engine->defaultData = _engine->metalRoughMaterial.write_material(
            _engine->_device, MaterialPass::MainColor, materialResources,
            _engine->globalDescriptorAllocator);
}
