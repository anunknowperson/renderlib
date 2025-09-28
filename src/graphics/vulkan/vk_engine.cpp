#include "graphics/vulkan/vk_engine.h"

#include <SDL_error.h>
#include <SDL_stdinc.h>
#include <SDL_video.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <functional>
#include <fmt/base.h>
#include <optional>
#include <random>
#include <system_error>

#include "core/Logging.h"
#include "core/config.h"
#include "graphics/vulkan/vk_descriptors.h"
#include "graphics/vulkan/DescriptorSetLayout.h"
#include "scene/Camera.h"


#define VMA_IMPLEMENTATION
#include <SDL_vulkan.h>
#include <VkBootstrap.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>
#include <vk_mem_alloc.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include "graphics/vulkan/vk_images.h"
#include "graphics/vulkan/vk_initializers.h"
#include "graphics/vulkan/vk_loader.h"
#include "graphics/vulkan/vk_pipelines.h"
#include "graphics/vulkan/vk_types.h"

VulkanEngine* loadedEngine = nullptr;

VulkanEngine& VulkanEngine::Get() {
    return *loadedEngine;
}

void VulkanEngine::Sampler::create(const VkDevice& pDevice,
            const VkSamplerCreateInfo* pCreateInfo,
            const VkAllocationCallbacks* pAllocator) {
    _device = pDevice;
    _allocator = pAllocator;
    vkCreateSampler(_device, pCreateInfo, pAllocator, &_sampler);
}

VulkanEngine::Sampler::~Sampler() {
    vkDestroySampler(_device, _sampler, _allocator);
}

VulkanEngine::Sampler::operator VkSampler() const { return _sampler; }

void VulkanEngine::destroy_image(const AllocatedImage& img) const {
    vkDestroyImageView(info.getLogicalDevice(), img.imageView, nullptr);
    vmaDestroyImage(info.getAllocator(), img.image, img.allocation);
}

void VulkanEngine::init_default_data() {
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
    testMeshes = loadGltfMeshes(this, path_to_assets).value();

    // 3 default textures, white, grey, black. 1 pixel each
    const uint32_t white = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
    AllocatedImage whiteImageData = create_image(&white, VkExtent3D{1, 1, 1}, VK_FORMAT_R8G8B8A8_UNORM,
                         VK_IMAGE_USAGE_SAMPLED_BIT);
    _whiteImage = std::make_unique<VulkanImage>(info.getAllocator(), info.getLogicalDevice(), whiteImageData);

    const uint32_t grey = glm::packUnorm4x8(glm::vec4(0.66f, 0.66f, 0.66f, 1));
    AllocatedImage greyImageData = create_image(&grey, VkExtent3D{1, 1, 1}, VK_FORMAT_R8G8B8A8_UNORM,
                         VK_IMAGE_USAGE_SAMPLED_BIT);
    _greyImage = std::make_unique<VulkanImage>(info.getAllocator(), info.getLogicalDevice(), greyImageData);

    const uint32_t black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 0));
    AllocatedImage blackImageData = create_image(&black, VkExtent3D{1, 1, 1}, VK_FORMAT_R8G8B8A8_UNORM,
                         VK_IMAGE_USAGE_SAMPLED_BIT);
    _blackImage = std::make_unique<VulkanImage>(static_cast<VmaAllocator>(info.getAllocator()), info.getLogicalDevice(), blackImageData);

    // checkerboard image
    const uint32_t magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
    std::array<uint32_t, 16 * 16> pixels{};  // for 16x16 checkerboard texture
    for (size_t x = 0; x < 16; x++) {
        for (size_t y = 0; y < 16; y++) {
            pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
        }
    }
    AllocatedImage errorImageData = create_image(pixels.data(), VkExtent3D{16, 16, 1},
                         VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);
    _errorCheckerboardImage = std::make_unique<VulkanImage>(info.getAllocator(), info.getLogicalDevice(), errorImageData);

    VkSamplerCreateInfo sampl = {.sType =
                                         VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};

    sampl.magFilter = VK_FILTER_NEAREST;
    sampl.minFilter = VK_FILTER_NEAREST;
    _defaultSamplerNearest.create(info.getLogicalDevice(), &sampl, nullptr);

    sampl.magFilter = VK_FILTER_LINEAR;
    sampl.minFilter = VK_FILTER_LINEAR;
    _defaultSamplerLinear.create(info.getLogicalDevice(), &sampl, nullptr);

    GLTFMetallic_Roughness::MaterialResources materialResources{};
    // default the material textures
    materialResources.colorImage = _whiteImage->get();
    materialResources.colorSampler = static_cast<VkSampler>(_defaultSamplerLinear);
    materialResources.metalRoughImage = _whiteImage->get();
    materialResources.metalRoughSampler = static_cast<VkSampler>(_defaultSamplerLinear);

    // set the uniform buffer for the material data
    const AllocatedBuffer materialConstants = create_buffer(
            sizeof(GLTFMetallic_Roughness::MaterialConstants),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

    // write the buffer
    auto* sceneUniformData =
            (GLTFMetallic_Roughness::MaterialConstants*)
                    materialConstants.allocation->GetMappedData();
    sceneUniformData->colorFactors = glm::vec4{1, 1, 1, 1};
    sceneUniformData->metal_rough_factors = glm::vec4{1, 0.5, 0, 0};

    // Store material constants buffer in managed buffers for automatic cleanup
    _managedBuffers.push_back(std::make_unique<VulkanBuffer>(info.getAllocator(), materialConstants));

    materialResources.dataBuffer = materialConstants.buffer;
    materialResources.dataBufferOffset = 0;

    defaultData = metalRoughMaterial.write_material(
            MaterialPass::MainColor, materialResources,
            globalDescriptorAllocator);
}

 void VulkanEngine::Imgui::initImguiPool() {
    // 1: create descriptor pool for IMGUI
    //  the size of the pool is very oversize, but it's copied from imgui demo
    //  itself.
    const VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};
    VkDescriptorPoolCreateInfo pool_info = {    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = 1000,
        .poolSizeCount = (uint32_t)std::size(pool_sizes),
        .pPoolSizes = pool_sizes};
    VK_CHECK(vkCreateDescriptorPool(_device, &pool_info, nullptr, &_imguiPool));
}

 void VulkanEngine::Imgui::init(const VkDevice& dev, SDL_Window* w,
     const VkInstance& pInstance, const VkPhysicalDevice& physicalDevice,
     const VkQueue& queue, const VkFormat* format) {
    _device = dev;
    initImguiPool();
    // 2: initialize imgui library
    // this initializes the core structures of imgui
    ImGui::CreateContext();
    // this initializes imgui for SDL
    ImGui_ImplSDL2_InitForVulkan(w);
    // this initializes imgui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info = {.Instance = pInstance,
                                           .PhysicalDevice = physicalDevice,
                                           .Device = _device,
                                           .Queue = queue,
                                           .DescriptorPool = _imguiPool,
                                           .MinImageCount = 3,
                                           .ImageCount = 3,
                                           .UseDynamicRendering = true};

    // dynamic rendering parameters for imgui to use
    init_info.PipelineRenderingCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
    init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats =
            format;

    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info);

    ImGui_ImplVulkan_CreateFontsTexture();
}

VulkanEngine::Imgui::~Imgui() {
    ImGui_ImplVulkan_Shutdown();
    vkDestroyDescriptorPool(_device, _imguiPool, nullptr);
}

void VulkanEngine::init_descriptors() {
    // create a descriptor pool that will hold 10 sets with 1 image each
    std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> sizes = {
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}};

    globalDescriptorAllocator.init(info.getLogicalDevice(), 10, sizes);

    // make the descriptor set layout for our compute draw
    _drawImageDescriptorLayout.create(info.getLogicalDevice(), VK_SHADER_STAGE_COMPUTE_BIT, {{0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE}});
    _gpuSceneDataDescriptorLayout.create(info.getLogicalDevice(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, {{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER}});
    _singleImageDescriptorLayout.create(info.getLogicalDevice(), VK_SHADER_STAGE_FRAGMENT_BIT, {{0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER}});

    // allocate a descriptor set for our draw image
    _drawImageDescriptors = globalDescriptorAllocator.allocate(
            info.getLogicalDevice(), _drawImageDescriptorLayout.set);

    DescriptorWriter writer;
    writer.write_image(0, _drawImage->imageView(), VK_NULL_HANDLE,
                       VK_IMAGE_LAYOUT_GENERAL,
                       VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);

    writer.update_set(info.getLogicalDevice(), _drawImageDescriptors);

    for (auto& _frame : command_buffers_container._frames) {
        // create a descriptor pool
        std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> frame_sizes = {
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4},
        };

        _frame._frameDescriptors = DescriptorAllocatorGrowable{};
        _frame._frameDescriptors.init(info.getLogicalDevice(), 1000, frame_sizes);

        // No need for deletion queue - frame descriptors will be cleaned up in cleanup()
    }
}

void VulkanEngine::init_pipelines() {
    pipelines.init(info.getLogicalDevice(), _singleImageDescriptorLayout.set, _drawImageDescriptorLayout.set, _drawImage->get());
    // Pipeline cleanup is handled automatically by the Pipelines object
    metalRoughMaterial.build_pipelines(this);
}

VulkanEngine::VulkanEngine(Camera& camera) : mainCamera(&camera) {
    // only one engine initialization is allowed with the application.
    assert(loadedEngine == nullptr);
    loadedEngine = this;
    init_swapchain();

    command_buffers.init_commands(this);

    command_buffers_container.init_sync_structures(this);
    init_descriptors();
    init_pipelines();
    _imgui.init(info.getLogicalDevice(), info.getWindow(), info.getInstance(), info.getPhysicalDevice(), info.getQueue(), &_swapchainImageFormat);
    init_default_data();

    mainCamera->velocity = glm::vec3(0.f);
    mainCamera->position = glm::vec3(0, 0, 5);

    mainCamera->pitch = 0;
    mainCamera->yaw = 0;

    const std::string structurePath = {std::string(ASSETS_DIR) +
                                       "/basicmesh.glb"};
    const auto structureFile = loadGltf(this, structurePath);

    assert(structureFile.has_value());
    loadedScenes["structure"] = *structureFile;

    _isInitialized = true;
}

void VulkanEngine::create_swapchain(uint32_t width, uint32_t height) {
    vkb::SwapchainBuilder swapchainBuilder{info.getPhysicalDevice(), info.getLogicalDevice(), info.getSurface()};

    _swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

    auto swap_ret =
            swapchainBuilder
                    //.use_default_format_selection()
                    .set_desired_format(VkSurfaceFormatKHR{
                            .format = _swapchainImageFormat,
                            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
                    // use vsync present mode
                    .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                    .set_desired_extent(width, height)
                    .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                    .build();

    if (!swap_ret) {
        LOGE("Failed to create swapchain. Error: {}",
             swap_ret.error().message());
    }

    vkb::Swapchain vkbSwapchain = swap_ret.value();

    _swapchainExtent = vkbSwapchain.extent;
    // store swapchain and its related images
    _swapchain = vkbSwapchain.swapchain;
    _swapchainImages = vkbSwapchain.get_images().value();
    _swapchainImageViews = vkbSwapchain.get_image_views().value();
}

void VulkanEngine::init_swapchain() {
    create_swapchain(_windowExtent.width, _windowExtent.height);

    // draw image size will match the window
    const VkExtent3D drawImageExtent = {_windowExtent.width,
                                        _windowExtent.height, 1};

    // hardcoding the draw format to 32-bit float
    VkFormat drawImageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;

    VkImageUsageFlags drawImageUsages{};
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    const VkImageCreateInfo rimg_info = vkinit::image_create_info(
            drawImageFormat, drawImageUsages, drawImageExtent);

    // for the draw image, we want to allocate it from gpu local memory
    VmaAllocationCreateInfo rimg_allocinfo = {};
    rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    rimg_allocinfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    AllocatedImage drawImageData{};
    drawImageData.imageFormat = drawImageFormat;
    drawImageData.imageExtent = drawImageExtent;

    // allocate and create the image
    vmaCreateImage(info.getAllocator(), &rimg_info, &rimg_allocinfo, &drawImageData.image,
                   &drawImageData.allocation, nullptr);

    // build an image-view for the draw image to use for rendering
    const VkImageViewCreateInfo rview_info = vkinit::imageview_create_info(
            drawImageFormat, drawImageData.image,
            VK_IMAGE_ASPECT_COLOR_BIT);

    VK_CHECK(vkCreateImageView(info.getLogicalDevice(), &rview_info, nullptr,
                               &drawImageData.imageView));

    // Create smart pointer for automatic cleanup
    _drawImage = std::make_unique<VulkanImage>(info.getAllocator(), info.getLogicalDevice(), drawImageData);

    // Create depth image
    VkExtent3D depthImageExtent = {
        _windowExtent.width,
        _windowExtent.height,
        1
    };

    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;

    VkImageCreateInfo dimg_info = vkinit::image_create_info(
        depthFormat,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        depthImageExtent);

    VmaAllocationCreateInfo dimg_allocinfo = {};
    dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    dimg_allocinfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    AllocatedImage depthImageData{};
    depthImageData.imageFormat = depthFormat;
    depthImageData.imageExtent = depthImageExtent;

    // allocate and create the depth image
    vmaCreateImage(info.getAllocator(), &dimg_info, &dimg_allocinfo, &depthImageData.image,
                   &depthImageData.allocation, nullptr);

    // build an image-view for the depth image
    VkImageViewCreateInfo dview_info = vkinit::imageview_create_info(
        depthFormat, depthImageData.image, VK_IMAGE_ASPECT_DEPTH_BIT);

    VK_CHECK(vkCreateImageView(info.getLogicalDevice(), &dview_info, nullptr,
                               &depthImageData.imageView));

    // Create smart pointer for automatic cleanup
    _depthImage = std::make_unique<VulkanImage>(info.getAllocator(), info.getLogicalDevice(), depthImageData);
}

void VulkanEngine::destroy_swapchain() {
    vkDestroySwapchainKHR(info.getLogicalDevice(), _swapchain, nullptr);

    // destroy swapchain resources
    for (const auto& _swapchainImageView : _swapchainImageViews) {
        vkDestroyImageView(info.getLogicalDevice(), _swapchainImageView, nullptr);
    }
}

 VulkanEngine::~VulkanEngine()
 {
    if (_isInitialized) {
        // make sure the gpu has stopped doing its things
        vkDeviceWaitIdle(info.getLogicalDevice());

        loadedScenes.clear();

        // Smart pointers will automatically clean up resources

        for (auto& _frame : command_buffers_container._frames) {
            // Destroy frame descriptors manually
            _frame._frameDescriptors.destroy_pools(info.getLogicalDevice());
        }
        globalDescriptorAllocator.destroy_pools(info.getLogicalDevice());

        destroy_swapchain();
    }

    // clear engine pointer
    loadedEngine = nullptr;
}

AllocatedBuffer VulkanEngine::create_buffer(size_t allocSize,
                                            VkBufferUsageFlags usage,
                                            VmaMemoryUsage memoryUsage) const {
    // allocate buffer
    VkBufferCreateInfo bufferInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferInfo.pNext = nullptr;
    bufferInfo.size = allocSize;

    bufferInfo.usage = usage;

    VmaAllocationCreateInfo vmallocinfo = {};
    vmallocinfo.usage = memoryUsage;
    vmallocinfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    AllocatedBuffer newBuffer{};

    // allocate the buffer
    VK_CHECK(vmaCreateBuffer(info.getAllocator(), &bufferInfo, &vmallocinfo,
                             &newBuffer.buffer, &newBuffer.allocation,
                             &newBuffer.info));

    return newBuffer;
}

void VulkanEngine::destroy_buffer(const AllocatedBuffer& buffer) const {
    vmaDestroyBuffer(info.getAllocator(), buffer.buffer, buffer.allocation);
}

GPUMeshBuffers VulkanEngine::uploadMesh(std::span<uint32_t> indices,
                                        std::span<Vertex> vertices) {
    const size_t vertexBufferSize = vertices.size() * sizeof(Vertex);
    const size_t indexBufferSize = indices.size() * sizeof(uint32_t);

    GPUMeshBuffers newSurface{};

    // create vertex buffer
    newSurface.vertexBuffer =
            create_buffer(vertexBufferSize,
                          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                  VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                          VMA_MEMORY_USAGE_GPU_ONLY);

    // find the address of the vertex buffer
    const VkBufferDeviceAddressInfo deviceAddressInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
            .buffer = newSurface.vertexBuffer.buffer};
    newSurface.vertexBufferAddress =
            vkGetBufferDeviceAddress(info.getLogicalDevice(), &deviceAddressInfo);

    // create index buffer
    newSurface.indexBuffer = create_buffer(
            indexBufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY);

    const AllocatedBuffer staging = create_buffer(
            vertexBufferSize + indexBufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

    void* data = staging.allocation->GetMappedData();

    // copy vertex buffer
    memcpy(data, vertices.data(), vertexBufferSize);
    // copy index buffer
    memcpy((char*)data + vertexBufferSize, indices.data(), indexBufferSize);

    command_buffers.immediate_submit([&](VkCommandBuffer cmd) {
        VkBufferCopy vertexCopy{0};
        vertexCopy.dstOffset = 0;
        vertexCopy.srcOffset = 0;
        vertexCopy.size = vertexBufferSize;

        vkCmdCopyBuffer(cmd, staging.buffer, newSurface.vertexBuffer.buffer, 1,
                        &vertexCopy);

        VkBufferCopy indexCopy{0};
        indexCopy.dstOffset = 0;
        indexCopy.srcOffset = vertexBufferSize;
        indexCopy.size = indexBufferSize;

        vkCmdCopyBuffer(cmd, staging.buffer, newSurface.indexBuffer.buffer, 1,
                        &indexCopy);
            },
            this);

    // Store mesh buffers in managed collections for automatic cleanup
    _managedBuffers.push_back(std::make_unique<VulkanBuffer>(info.getAllocator(), newSurface.vertexBuffer));
    _managedBuffers.push_back(std::make_unique<VulkanBuffer>(info.getAllocator(), newSurface.indexBuffer));
    destroy_buffer(staging);

    return newSurface;
}

void VulkanEngine::draw_background(VkCommandBuffer cmd) const {
    // bind the gradient drawing compute pipeline

    pipelines.gradientPipeline->bind(cmd);

    // bind descriptor sets
    pipelines.gradientPipeline->bindDescriptorSets(cmd, &_drawImageDescriptors, 1);

    // dispatch the compute shader
    pipelines.gradientPipeline->dispatch(cmd,
        static_cast<uint32_t>(std::ceil(_drawExtent.width / 16.0)),
        static_cast<uint32_t>(std::ceil(_drawExtent.height / 16.0)),
        1);
}

void VulkanEngine::draw_imgui(VkCommandBuffer cmd,
                              VkImageView targetImageView) const {
    const VkRenderingAttachmentInfo colorAttachment = vkinit::attachment_info(
            targetImageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    const VkRenderingInfo renderInfo =
            vkinit::rendering_info(_swapchainExtent, &colorAttachment, nullptr);

    vkCmdBeginRendering(cmd, &renderInfo);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

    vkCmdEndRendering(cmd);
}

void VulkanEngine::draw_geometry(VkCommandBuffer cmd) {
    // allocate a new uniform buffer for the scene data
    AllocatedBuffer gpuSceneDataBuffer = create_buffer(
            sizeof(GPUSceneData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU);

    // add it to the current frame's managed buffers for automatic cleanup
    get_current_frame()._frameBuffers.push_back(
        std::make_unique<VulkanBuffer>(info.getAllocator(), gpuSceneDataBuffer));

    // write the buffer
    auto* sceneUniformData =
            (GPUSceneData*)gpuSceneDataBuffer.allocation->GetMappedData();
    *sceneUniformData = sceneData;

    // create a descriptor set that binds that buffer and update it
    VkDescriptorSet globalDescriptor =
            get_current_frame()._frameDescriptors.allocate(
                    info.getLogicalDevice(), _gpuSceneDataDescriptorLayout.set);

    DescriptorWriter writer;
    writer.write_buffer(0, gpuSceneDataBuffer.buffer, sizeof(GPUSceneData), 0,
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    writer.update_set(info.getLogicalDevice(), globalDescriptor);

    VkRenderingAttachmentInfo colorAttachment = vkinit::attachment_info(
            _drawImage->imageView(), nullptr, VK_IMAGE_LAYOUT_GENERAL);

    VkRenderingInfo renderInfo =
            vkinit::rendering_info(_drawExtent, &colorAttachment, nullptr);
    vkCmdBeginRendering(cmd, &renderInfo);

    pipelines.trianglePipeline->bind(cmd);

    // set dynamic viewport and scissor
    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<float>(_drawExtent.width);
    viewport.height = static_cast<float>(_drawExtent.height);
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = _drawExtent.width;
    scissor.extent.height = _drawExtent.height;

    vkCmdSetScissor(cmd, 0, 1, &scissor);

    // bind a texture
    VkDescriptorSet imageSet = get_current_frame()._frameDescriptors.allocate(
            info.getLogicalDevice(), _singleImageDescriptorLayout.set);
    DescriptorWriter single_image_writer;
    single_image_writer.write_image(0, _errorCheckerboardImage->imageView(),
                                    static_cast<VkSampler>(_defaultSamplerNearest),
                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    single_image_writer.update_set(info.getLogicalDevice(), imageSet);

    pipelines.meshPipeline->bindDescriptorSets(cmd, &imageSet, 1);

    for (const auto& [indexCount, firstIndex, indexBuffer, material, transform,
                      vertexBufferAddress] : mainDrawContext.OpaqueSurfaces) {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          material->pipeline->pipeline);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                material->pipeline->layout, 0, 1,
                                &globalDescriptor, 0, nullptr);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                material->pipeline->layout, 1, 1,
                                &material->materialSet, 0, nullptr);

        vkCmdBindIndexBuffer(cmd, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        GPUDrawPushConstants pushConstants{};
        pushConstants.vertexBuffer = vertexBufferAddress;
        pushConstants.worldMatrix = transform;
        vkCmdPushConstants(cmd, material->pipeline->layout,
                           VK_SHADER_STAGE_VERTEX_BIT, 0,
                           sizeof(GPUDrawPushConstants), &pushConstants);

        vkCmdDrawIndexed(cmd, indexCount, 1, firstIndex, 0, 0);
    }

    vkCmdEndRendering(cmd);
}

void VulkanEngine::draw() {
    update_scene();

    // wait until the gpu has finished rendering the last frame. Timeout of 1
    // second
    VK_CHECK(vkWaitForFences(info.getLogicalDevice(), 1, get_current_frame()._renderFence->getPtr(),
                             true, 1000000000));

    // Clear frame buffers instead of flushing deletion queue
    get_current_frame()._frameBuffers.clear();
    get_current_frame()._frameDescriptors.clear_pools(info.getLogicalDevice());

    VK_CHECK(vkResetFences(info.getLogicalDevice(), 1, get_current_frame()._renderFence->getPtr()));

    // request image from the swapchain
    uint32_t swapchainImageIndex;
    const VkResult e =
            vkAcquireNextImageKHR(info.getLogicalDevice(), _swapchain, 1000000000,
                                  get_current_frame()._swapchainSemaphore->get(),
                                  nullptr, &swapchainImageIndex);
    if (e == VK_ERROR_OUT_OF_DATE_KHR) {
        resize_requested = true;
        return;
    }

    // naming it cmd for shorter writing
    const VkCommandBuffer cmd = get_current_frame()._mainCommandBuffer;

    // now that we are sure that the commands finished executing, we can safely
    // reset the command buffer to begin recording again.
    VK_CHECK(vkResetCommandBuffer(cmd, 0));

    // begin the command buffer recording. We will use this command buffer
    // exactly once, so we want to let vulkan know that
    const VkCommandBufferBeginInfo cmdBeginInfo =
            vkinit::command_buffer_begin_info(
                    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    _drawExtent.height = static_cast<uint32_t>(
            (float)std::min(_swapchainExtent.height,
                            _drawImage->get().imageExtent.height) *
            renderScale);
    _drawExtent.width = static_cast<uint32_t>(
            (float)std::min(_swapchainExtent.width,
                            _drawImage->get().imageExtent.width) *
            renderScale);

    VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    // transition our main draw image into general layout, so we can write into
    // it, we will overwrite it all, so we don't care about what was the older
    // layout
    vkutil::transition_image(cmd, _drawImage->image(), VK_IMAGE_LAYOUT_UNDEFINED,
                             VK_IMAGE_LAYOUT_GENERAL);

    draw_background(cmd);

    vkutil::transition_image(cmd, _drawImage->image(), VK_IMAGE_LAYOUT_GENERAL,
                             VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    draw_geometry(cmd);

    // transition the draw image and the swapchain image into their correct
    // transfer layouts
    vkutil::transition_image(cmd, _drawImage->image(),
                             VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                             VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex],
                             VK_IMAGE_LAYOUT_UNDEFINED,
                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // execute a copy from the draw image into the swapchain
    vkutil::copy_image_to_image(cmd, _drawImage->image(),
                                _swapchainImages[swapchainImageIndex],
                                _drawExtent, _swapchainExtent);

    // set swapchain image layout to Present, so we can show it on the screen
    vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex],
                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                             VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    // draw imgui into the swapchain image
    draw_imgui(cmd, _swapchainImageViews[swapchainImageIndex]);

    // set swapchain image layout to Present, so we can draw it
    // vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex],
    // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    // finalize the command buffer (we can no longer add commands, but it can
    // now be executed)
    VK_CHECK(vkEndCommandBuffer(cmd));

    // prepare the submission to the queue.
    // we want to wait on the _presentSemaphore, as that semaphore is signaled
    // when the swapchain is ready we will signal the _renderSemaphore, to
    // signal that rendering has finished

    const VkCommandBufferSubmitInfo cmdinfo =
            vkinit::command_buffer_submit_info(cmd);

    const VkSemaphoreSubmitInfo waitInfo = vkinit::semaphore_submit_info(
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
            get_current_frame()._swapchainSemaphore->get());
    const VkSemaphoreSubmitInfo signalInfo =
            vkinit::semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                                          get_current_frame()._renderSemaphore->get());

    const VkSubmitInfo2 submit =
            vkinit::submit_info(&cmdinfo, &signalInfo, &waitInfo);

    // submit command buffer to the queue and execute it.
    //  _renderFence will now block until the graphic commands finish execution
    VK_CHECK(vkQueueSubmit2(info.getQueue(), 1, &submit,
                            get_current_frame()._renderFence->get()));

    // prepare present
    //  this will put the image we just rendered to into the visible window.
    //  we want to wait on the _renderSemaphore for that,
    //  as its necessary that drawing commands have finished before the image is
    //  displayed to the user
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.pSwapchains = &_swapchain;
    presentInfo.swapchainCount = 1;

    presentInfo.pWaitSemaphores = get_current_frame()._renderSemaphore->getPtr();
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &swapchainImageIndex;

    VkResult presentResult = vkQueuePresentKHR(info.getQueue(), &presentInfo);
    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR) {
        resize_requested = true;
    }

    // increase the number of frames drawn
    _frameNumber++;
}

void VulkanEngine::resize_swapchain() {
    vkDeviceWaitIdle(info.getLogicalDevice());

    destroy_swapchain();

    int w, h;
    SDL_GetWindowSize(info.getWindow(), &w, &h);
    _windowExtent.width = static_cast<uint32_t>(w);
    _windowExtent.height = static_cast<uint32_t>(h);

    create_swapchain(_windowExtent.width, _windowExtent.height);

    resize_requested = false;
}

void VulkanEngine::update() {
    if (resize_requested) {
        resize_swapchain();
    }

    draw();
}

AllocatedImage VulkanEngine::create_image(VkExtent3D size, VkFormat format,
                                          VkImageUsageFlags usage,
                                          bool mipmapped) const {
    AllocatedImage newImage{};
    newImage.imageFormat = format;
    newImage.imageExtent = size;

    VkImageCreateInfo img_info = vkinit::image_create_info(format, usage, size);
    if (mipmapped) {
        img_info.mipLevels = static_cast<uint32_t>(std::floor(std::log2(
                                     std::max(size.width, size.height)))) +
                             1;
    }

    // always allocate images on dedicated GPU memory
    VmaAllocationCreateInfo allocinfo = {};
    allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocinfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // allocate and create the image
    VK_CHECK(vmaCreateImage(info.getAllocator(), &img_info, &allocinfo, &newImage.image,
                            &newImage.allocation, nullptr));

    // if the format is a depth format, we will need to have it use the correct
    // aspect flag
    VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
    if (format == VK_FORMAT_D32_SFLOAT) {
        aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
    }

    // build an image-view for the image
    VkImageViewCreateInfo view_info =
            vkinit::imageview_create_info(format, newImage.image, aspectFlag);
    view_info.subresourceRange.levelCount = img_info.mipLevels;

    VK_CHECK(vkCreateImageView(info.getLogicalDevice(), &view_info, nullptr,
                               &newImage.imageView));

    return newImage;
}

AllocatedImage VulkanEngine::create_image(const void* data, VkExtent3D size,
                                          VkFormat format,
                                          VkImageUsageFlags usage,
                                          bool mipmapped) const {
    const size_t data_size = size.depth * size.width * size.height * 4;
    const AllocatedBuffer uploadbuffer =
            create_buffer(data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          VMA_MEMORY_USAGE_CPU_TO_GPU);

    memcpy(uploadbuffer.info.pMappedData, data, data_size);

    const AllocatedImage new_image =
            create_image(size, format,
                         usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                 VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                         mipmapped);

    command_buffers.immediate_submit([&](VkCommandBuffer cmd) {
        vkutil::transition_image(cmd, new_image.image,
                                 VK_IMAGE_LAYOUT_UNDEFINED,
                                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VkBufferImageCopy copyRegion = {};
        copyRegion.bufferOffset = 0;
        copyRegion.bufferRowLength = 0;
        copyRegion.bufferImageHeight = 0;

        copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.imageSubresource.mipLevel = 0;
        copyRegion.imageSubresource.baseArrayLayer = 0;
        copyRegion.imageSubresource.layerCount = 1;
        copyRegion.imageExtent = size;

        // copy the buffer into the image
        vkCmdCopyBufferToImage(cmd, uploadbuffer.buffer, new_image.image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                               &copyRegion);

        vkutil::transition_image(cmd, new_image.image,
                                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            },
            (VulkanEngine*)this);

    destroy_buffer(uploadbuffer);

    return new_image;
}


void MeshNode::Draw(const glm::mat4& topMatrix, DrawContext& ctx) {
    const glm::mat4 nodeMatrix = topMatrix * worldTransform;

    for (auto& [startIndex, count, material] : mesh->surfaces) {
        RenderObject def{};
        def.indexCount = count;
        def.firstIndex = startIndex;
        def.indexBuffer = mesh->meshBuffers.indexBuffer.buffer;
        def.material = &material->data;

        def.transform = nodeMatrix;
        def.vertexBufferAddress = mesh->meshBuffers.vertexBufferAddress;

        ctx.OpaqueSurfaces.push_back(def);
    }

    ENode::Draw(topMatrix, ctx);
}

void VulkanEngine::update_scene() {
    mainCamera->update();

    const glm::mat4 view = mainCamera->getViewMatrix();

    glm::mat4 projection = glm::perspective(
            glm::radians(70.f),
            (float)_windowExtent.width / (float)_windowExtent.height, 0.1f,
            10000.f);

    // to opengl and gltf axis
    projection[1][1] *= -1;

    sceneData.view = view;
    sceneData.proj = projection;
    sceneData.viewproj = projection * view;

    mainDrawContext.OpaqueSurfaces.clear();

    sceneData.ambientColor = glm::vec4(.1f);
    sceneData.sunlightColor = glm::vec4(1.f);
    sceneData.sunlightDirection = glm::vec4(0, 1, 0.5, 1.f);

    for (const auto& [key, mesh] : meshes) {
        const std::shared_ptr<LoadedGLTF> loadedMesh = mesh;
        loadedMesh->Draw(transforms[key], mainDrawContext);
    }
}

int64_t VulkanEngine::registerMesh(const std::string& filePath) {
    std::random_device rd;

    // Use the Mersenne Twister engine for high-quality random numbers
    std::mt19937_64 generator(rd());

    // Create a uniform distribution for int64_t
    std::uniform_int_distribution<int64_t> distribution;

    // Generate and print a random int64_t value
    const int64_t random_int64 = distribution(generator);

    const std::string structurePath = {std::string(ASSETS_DIR) + filePath};
    const auto structureFile = loadGltf(this, structurePath);

    assert(structureFile.has_value());

    meshes[random_int64] = *structureFile;
    transforms[random_int64] = glm::mat4(1.0f);

    return random_int64;
}

void VulkanEngine::unregisterMesh(int64_t id) {
    if (meshes.contains(id)) {
        meshes.erase(id);
        transforms.erase(id);
    }
}

void VulkanEngine::setMeshTransform(int64_t id, glm::mat4 mat) {
    transforms[id] = mat;
}
