#pragma once

#include <random>
#include <utility>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float4.hpp>
#include <memory>
#include <ranges>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>
#include <vk_mem_alloc.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include "core/Mesh.h"
#include "scene/Camera.h"
#include "vk_descriptors.h"
#include "vk_loader.h"
#include "vk_types.h"
#include "vk_command_buffers.h" 

class Camera;
class VulkanEngine;
struct DrawContext;
struct LoadedGLTF;
struct MeshAsset;

constexpr unsigned int FRAME_OVERLAP = 2;

struct GLTFMetallic_Roughness {
    MaterialPipeline opaquePipeline;
    MaterialPipeline transparentPipeline;

    VkDescriptorSetLayout materialLayout;

    struct MaterialConstants {
        glm::vec4 colorFactors;
        glm::vec4 metal_rough_factors;
        // padding, we need it anyway for uniform buffers
        glm::vec4 extra[14];
    };

    struct MaterialResources {
        AllocatedImage colorImage;
        VkSampler colorSampler;
        AllocatedImage metalRoughImage;
        VkSampler metalRoughSampler;
        VkBuffer dataBuffer;
        uint32_t dataBufferOffset;
    };

    DescriptorWriter writer;

    void build_pipelines(VulkanEngine* engine);
    void clear_resources(VkDevice device);

    MaterialInstance write_material(
            VkDevice device, MaterialPass pass,
            const MaterialResources& resources,
            DescriptorAllocatorGrowable& descriptorAllocator);
};

struct DeletionQueue {
    std::deque<std::function<void()>> deletors;

    void push_function(std::function<void()>&& function) {
        deletors.push_back(function);
    }

    void flush() {
        // reverse iterate the deletion queue to execute all the functions
        for (auto& deletor : std::ranges::reverse_view(deletors)) {
            deletor();  // call functors
        }

        deletors.clear();
    }
};

struct FrameData {
    VkCommandPool _commandPool;
    VkCommandBuffer _mainCommandBuffer;

    VkSemaphore _swapchainSemaphore, _renderSemaphore;
    VkFence _renderFence;

    DeletionQueue _deletionQueue;
    DescriptorAllocatorGrowable _frameDescriptors;
};

struct GPUSceneData {
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewproj;
    glm::vec4 ambientColor;
    glm::vec4 sunlightDirection;  // w for sun power
    glm::vec4 sunlightColor;
};

struct MeshNode : public ENode {
    MeshNode() = default;

    std::shared_ptr<MeshAsset> mesh;

    void Draw(const glm::mat4& topMatrix, DrawContext& ctx) override;
};

struct RenderObject {
    uint32_t indexCount;
    uint32_t firstIndex;
    VkBuffer indexBuffer;

    MaterialInstance* material;

    glm::mat4 transform;
    VkDeviceAddress vertexBufferAddress;
};

struct DrawContext {
    std::vector<RenderObject> OpaqueSurfaces;
};

struct VulkanContext {
    VkExtent2D windowExtent{2560, 1440};
    VkSurfaceKHR surface; // Vulkan window surface
    VkPhysicalDevice chosenGPU; // GPU chosen as the default device
    VkDevice device; // Vulkan device for commands

    AllocatedImage drawImage;

    DeletionQueue mainDeletionQueue;

    bool resize_requested;
};

class ISwapchainController {
public:
    virtual ~ISwapchainController() = default;
    virtual void create_swapchain(uint32_t width, uint32_t height) = 0;
    virtual void destroy_swapchain() = 0;
    virtual void resize_swapchain() = 0;

    virtual const VkFormat* get_swapchain_image_format() const = 0;
    virtual VkExtent2D get_swapchain_extent() const = 0;
    virtual VkSwapchainKHR get_swapchain() const = 0;
    virtual const VkSwapchainKHR* get_swapchain_ptr() const = 0;
    virtual std::vector<VkImage> get_swapchain_images() const = 0;
    virtual VkImage get_swapchain_image_by_index(const uint32_t i) const = 0;
    virtual VkImageView get_swapchain_image_view_by_index(const uint32_t i) const = 0;
    virtual std::vector<VkImageView> get_swapchain_image_views() const = 0;
};

class SwapchainController : public ISwapchainController {
public:
    SwapchainController(std::shared_ptr<VulkanContext> ctx,
                        VmaAllocator allocator,
                        SDL_Window* window);

    void create_swapchain(uint32_t width, uint32_t height) override;
    void destroy_swapchain() override;
    void resize_swapchain() override;

    [[nodiscard]] const VkFormat* get_swapchain_image_format() const  override {
        return &_swapchainImageFormat;
    }

    [[nodiscard]] VkExtent2D get_swapchain_extent() const override {
        return _swapchainExtent;
    }

    [[nodiscard]] VkSwapchainKHR get_swapchain() const override {
        return _swapchain;
    }

    [[nodiscard]] const VkSwapchainKHR* get_swapchain_ptr() const override {
        return &_swapchain;
    }

    [[nodiscard]] std::vector<VkImage> get_swapchain_images() const override {
        return _swapchainImages;
    }

    [[nodiscard]] VkImage get_swapchain_image_by_index(const uint32_t i) const override {
        return _swapchainImages[i];
    }

    [[nodiscard]] VkImageView get_swapchain_image_view_by_index(const uint32_t i) const override {
        return _swapchainImageViews[i];
    }

    std::vector<VkImageView> get_swapchain_image_views() const  override {
        return _swapchainImageViews;
    }
private:
    std::shared_ptr<VulkanContext> vCtxP;

    VmaAllocator _allocator;

    VkFormat _swapchainImageFormat;
    VkExtent2D _swapchainExtent;
    VkSwapchainKHR _swapchain;
    std::vector<VkImage> _swapchainImages;
    std::vector<VkImageView> _swapchainImageViews;

    SDL_Window* _window{nullptr};

};

class VulkanEngine {
public:
    int64_t registerMesh(std::string filePath);
    void unregisterMesh(int64_t id);

    void setMeshTransform(int64_t id, glm::mat4 mat);

    std::unordered_map<int64_t, std::shared_ptr<LoadedGLTF>> meshes;

    std::unordered_map<int64_t, glm::mat4> transforms;

    std::unordered_map<std::string, std::shared_ptr<LoadedGLTF>> loadedScenes;

    Camera* mainCamera;

    DrawContext mainDrawContext;
    std::shared_ptr<VulkanContext> vCtx;
    std::unordered_map<std::string, std::shared_ptr<ENode>> loadedNodes;

    void update_scene();

    FrameData _frames[FRAME_OVERLAP];

    FrameData& get_current_frame() {
        return _frames[_frameNumber % FRAME_OVERLAP];
    };

    VkQueue _graphicsQueue;
    uint32_t _graphicsQueueFamily;

    bool _isInitialized{false};
    unsigned int _frameNumber{0};
    bool stop_rendering{false};

    SDL_Window* _window{nullptr};

    static VulkanEngine& Get();

    // initializes everything in the engine
    void init(struct SDL_Window* window);

    // shuts down the engine
    void cleanup();

    // draw loop
    void draw();

    // run main loop
    void update();

    VkInstance _instance;                       // Vulkan library handle
    VkDebugUtilsMessengerEXT _debug_messenger;  // Vulkan debug output handle

    VmaAllocator _allocator;

    AllocatedImage _depthImage;
    VkExtent2D _drawExtent;
    float renderScale = 1.f;

    DescriptorAllocatorGrowable globalDescriptorAllocator;

    VkDescriptorSet _drawImageDescriptors;
    VkDescriptorSetLayout _drawImageDescriptorLayout;

    VkPipeline _gradientPipeline;
    VkPipelineLayout _gradientPipelineLayout;

    // immediate submit structures
    VkFence _immFence;
    VkCommandBuffer _immCommandBuffer;
    VkCommandPool _immCommandPool;

    VkPipelineLayout _trianglePipelineLayout;
    VkPipeline _trianglePipeline;

    VkPipelineLayout _meshPipelineLayout;
    VkPipeline _meshPipeline;

    GPUMeshBuffers rectangle;

    GPUMeshBuffers uploadMesh(std::span<uint32_t> indices,
                              std::span<Vertex> vertices);

    std::vector<std::shared_ptr<MeshAsset>> testMeshes;

    GPUSceneData sceneData;

    VkDescriptorSetLayout _gpuSceneDataDescriptorLayout;

    AllocatedImage create_image(VkExtent3D size, VkFormat format,
                                VkImageUsageFlags usage,
                                bool mipmapped = false) const;
    AllocatedImage create_image(const void* data, VkExtent3D size,
                                VkFormat format, VkImageUsageFlags usage,
                                bool mipmapped = false) const;
    void destroy_image(const AllocatedImage& img) const;

    AllocatedImage _whiteImage;
    AllocatedImage _blackImage;
    AllocatedImage _greyImage;
    AllocatedImage _errorCheckerboardImage;

    VkSampler _defaultSamplerLinear;
    VkSampler _defaultSamplerNearest;

    VkDescriptorSetLayout _singleImageDescriptorLayout;

    MaterialInstance defaultData;
    GLTFMetallic_Roughness metalRoughMaterial;

    AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage,
                                  VmaMemoryUsage memoryUsage) const;

private:
    std::unique_ptr<ISwapchainController> _swapchainСontrollerP;

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                  void* pUserData);

    void init_vulkan();
    void init_commands();
    void init_sync_structures();

    void draw_background(VkCommandBuffer cmd);

    void init_descriptors();

    void init_pipelines();
    void init_background_pipelines();
    void init_imgui();

    void init_triangle_pipeline();

    void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView) const;

    void draw_geometry(VkCommandBuffer cmd);

    void destroy_buffer(const AllocatedBuffer& buffer) const;

    void init_mesh_pipeline();

    void init_default_data();
};