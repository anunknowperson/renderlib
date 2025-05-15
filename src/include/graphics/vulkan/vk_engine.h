#pragma once

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

// Core includes for the Vulkan engine
#include "ComputePipeline.h"
#include "pipelines.h"  // Contains GLTFMetallic_Roughness definition
#include "vk_command_buffers.h"
#include "vk_descriptors.h"
#include "vk_types.h"

// Forward declarations
class Camera;
// Note: VulkanEngine is the class we are defining, no forward declaration
// needed here
struct DrawContext;
struct LoadedGLTF;
struct MeshAsset;

// Maximum number of frames that can be processed concurrently
constexpr unsigned int FRAME_OVERLAP = 2;

// Note: GLTFMetallic_Roughness struct definition is likely in pipelines.h

/**
 * DeletionQueue - Helper structure for managing resource cleanup
 */
struct DeletionQueue {
    std::deque<std::function<void()>> deletors;

    void push_function(std::function<void()>&& function) {
        deletors.push_back(function);
    }

    void flush() {
        // Reverse iterate the deletion queue to execute all the functions
        for (auto& deletor : std::ranges::reverse_view(deletors)) {
            deletor();  // Call functors
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

/**
 * VulkanEngine - Main renderer class
 */
class VulkanEngine {
public:
    // PUBLIC METHODS

    // Static Methods
    static VulkanEngine& Get();

    // Initialization and Cleanup
    void init(struct SDL_Window* window);
    void cleanup();

    // Main Loop
    void update();
    void draw();

    // Current frame access
    FrameData& get_current_frame();

    // Core Functionality - Scene and Mesh Management
    int64_t registerMesh(const std::string& filePath);
    void unregisterMesh(int64_t id);
    void setMeshTransform(int64_t id, glm::mat4 mat);
    void update_scene();
    GPUMeshBuffers uploadMesh(std::span<uint32_t> indices,
                              std::span<Vertex> vertices);

    // Core Functionality - Resource Creation
    AllocatedImage create_image(VkExtent3D size, VkFormat format,
                                VkImageUsageFlags usage,
                                bool mipmapped = false) const;
    AllocatedImage create_image(const void* data, VkExtent3D size,
                                VkFormat format, VkImageUsageFlags usage,
                                bool mipmapped = false) const;
    AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage,
                                  VmaMemoryUsage memoryUsage) const;

    // Resource management methods (public destruction for images)
    void destroy_image(const AllocatedImage& img) const;

    // PUBLIC FIELDS

    // Display settings
    VkExtent2D _windowExtent{2560, 1440};
    float renderScale = 1.f;
    bool resize_requested;
    struct SDL_Window* _window{nullptr};  // Interaction object

    // Core components (Managers and shared Vulkan objects)
    Pipelines pipelines;
    CommandBuffers command_buffers;
    DescriptorAllocatorGrowable globalDescriptorAllocator;

    // Immediate command submission structures
    VkFence _immFence;
    VkCommandBuffer _immCommandBuffer;
    VkCommandPool _immCommandPool;

    // Scene and object data
    GPUSceneData sceneData;
    GPUMeshBuffers rectangle;
    DrawContext mainDrawContext;
    Camera* mainCamera;  // Interaction object

    // Materials (Struct definition is elsewhere, likely pipelines.h)
    MaterialInstance defaultData;
    GLTFMetallic_Roughness metalRoughMaterial;

    // Collections of loaded objects (Asset management)
    std::unordered_map<int64_t, std::shared_ptr<LoadedGLTF>> meshes;
    std::unordered_map<int64_t, glm::mat4> transforms;
    std::unordered_map<std::string, std::shared_ptr<LoadedGLTF>> loadedScenes;
    std::unordered_map<std::string, std::shared_ptr<ENode>> loadedNodes;
    std::vector<std::shared_ptr<MeshAsset>> testMeshes;

private:
    // PRIVATE METHODS

    // Vulkan debug callback function
    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                  void* pUserData);

    // Internal initialization methods
    void init_vulkan();
    void init_swapchain();
    void init_sync_structures();
    void init_descriptors();
    void init_pipelines();
    void init_imgui();
    void init_triangle_pipeline();  // Added from e10dd8c
    void init_mesh_pipeline();
    void init_default_data();

    // Rendering methods
    void draw_background(VkCommandBuffer cmd) const;
    void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView) const;
    void draw_geometry(VkCommandBuffer cmd);

    // Swapchain management methods
    void create_swapchain(uint32_t width, uint32_t height);
    void destroy_swapchain();
    void resize_swapchain();

    // Memory management method (private destruction for buffers)
    void destroy_buffer(const AllocatedBuffer& buffer) const;

    // PRIVATE FIELDS

    // State flags
    bool _isInitialized{false};
    bool stop_rendering{false};

    // Frame data
    FrameData _frames[FRAME_OVERLAP];
    unsigned int _frameNumber{0};

    // Vulkan instances and devices
    VkInstance _instance;
    VkDebugUtilsMessengerEXT _debug_messenger;
    VkPhysicalDevice _chosenGPU;
    VkDevice _device;
    VkSurfaceKHR _surface;

    // Queues and queue families
    VkQueue _graphicsQueue;
    uint32_t _graphicsQueueFamily;

    // Swapchain and its resources
    VkSwapchainKHR _swapchain;
    VkFormat _swapchainImageFormat;
    std::vector<VkImage> _swapchainImages;
    std::vector<VkImageView> _swapchainImageViews;
    VkExtent2D _swapchainExtent;
    VkExtent2D _drawExtent;

    // Memory and object management
    VmaAllocator _allocator;
    DeletionQueue _mainDeletionQueue;

    // Image resources (private images/textures)
    AllocatedImage _drawImage;
    AllocatedImage _depthImage;
    AllocatedImage _whiteImage;
    AllocatedImage _blackImage;
    AllocatedImage _greyImage;
    AllocatedImage _errorCheckerboardImage;

    // Samplers
    VkSampler _defaultSamplerLinear;
    VkSampler _defaultSamplerNearest;

    // Descriptors (private descriptor sets/layouts)
    VkDescriptorSet _drawImageDescriptors;
    VkDescriptorSetLayout _drawImageDescriptorLayout;
    VkDescriptorSetLayout _gpuSceneDataDescriptorLayout;
    VkDescriptorSetLayout _singleImageDescriptorLayout;

    // Pipeline objects (private pipelines/layouts)
    VkPipeline _gradientPipeline;
    VkPipelineLayout _gradientPipelineLayout;
};