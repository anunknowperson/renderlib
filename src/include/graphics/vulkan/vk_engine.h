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

#include "ComputePipeline.h"
#include "pipelines.h"  // Contains GLTFMetallic_Roughness definition
#include "vk_command_buffers.h"
#include "vk_descriptors.h"
#include "vk_types.h"

class Camera;
struct DrawContext;
struct LoadedGLTF;
struct MeshAsset;

constexpr unsigned int FRAME_OVERLAP = 2;

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

class VulkanEngine {
public:
    // PUBLIC METHODS

    // Singleton accessor
    static VulkanEngine& Get();

    // Main lifecycle methods
    void init(struct SDL_Window* window);
    void cleanup();
    void draw();
    void update();

    // Current frame access
    FrameData& get_current_frame() {
        return _frames[_frameNumber % FRAME_OVERLAP];
    }

    // Mesh management methods
    int64_t registerMesh(const std::string& filePath);
    void unregisterMesh(int64_t id);
    void setMeshTransform(int64_t id, glm::mat4 mat);

    // Scene management methods
    void update_scene();

    // Resource creation methods (can be called from other classes)
    AllocatedImage create_image(VkExtent3D size, VkFormat format,
                                VkImageUsageFlags usage,
                                bool mipmapped = false) const;
    AllocatedImage create_image(const void* data, VkExtent3D size,
                                VkFormat format, VkImageUsageFlags usage,
                                bool mipmapped = false) const;
    AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage,
                                  VmaMemoryUsage memoryUsage) const;

    // Resource management methods
    void destroy_image(const AllocatedImage& img) const;
    GPUMeshBuffers uploadMesh(std::span<uint32_t> indices,
                              std::span<Vertex> vertices);

    // PUBLIC FIELDS

    // Display settings
    VkExtent2D _windowExtent{2560, 1440};
    float renderScale = 1.f;
    bool resize_requested;

    // Main interaction objects with other systems
    Camera* mainCamera;
    struct SDL_Window* _window{nullptr};

    // State flags
    bool _isInitialized{false};
    bool stop_rendering{false};

    // Core components
    Pipelines pipelines;
    CommandBuffers command_buffers;

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

    // Memory management method
    void destroy_buffer(const AllocatedBuffer& buffer) const;

    // PRIVATE FIELDS

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

    // Image resources
    AllocatedImage _drawImage;
    AllocatedImage _depthImage;
    AllocatedImage _whiteImage;
    AllocatedImage _blackImage;
    AllocatedImage _greyImage;
    AllocatedImage _errorCheckerboardImage;

    // Samplers
    VkSampler _defaultSamplerLinear;
    VkSampler _defaultSamplerNearest;

    // Descriptors
    VkDescriptorSet _drawImageDescriptors;
    VkDescriptorSetLayout _drawImageDescriptorLayout;
    VkDescriptorSetLayout _gpuSceneDataDescriptorLayout;
    VkDescriptorSetLayout _singleImageDescriptorLayout;
    DescriptorAllocatorGrowable globalDescriptorAllocator;

    // Pipeline objects
    VkPipeline _gradientPipeline;
    VkPipelineLayout _gradientPipelineLayout;

    // Immediate command submission structures
    VkFence _immFence;
    VkCommandBuffer _immCommandBuffer;
    VkCommandPool _immCommandPool;

    // Scene and object data
    GPUSceneData sceneData;
    GPUMeshBuffers rectangle;
    DrawContext mainDrawContext;

    // Materials
    MaterialInstance defaultData;
    GLTFMetallic_Roughness metalRoughMaterial;

    // Collections of loaded objects
    std::unordered_map<int64_t, std::shared_ptr<LoadedGLTF>> meshes;
    std::unordered_map<int64_t, glm::mat4> transforms;
    std::unordered_map<std::string, std::shared_ptr<LoadedGLTF>> loadedScenes;
    std::unordered_map<std::string, std::shared_ptr<ENode>> loadedNodes;
    std::vector<std::shared_ptr<MeshAsset>> testMeshes;
};