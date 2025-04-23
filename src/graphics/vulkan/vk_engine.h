#pragma once

#include <deque>
#include <functional>
#include <memory>
#include <random>
#include <span>
#include <unordered_map>
#include <vector>

#include "core/Mesh.h"
#include "scene/Camera.h"
#include "vk_descriptors.h"
#include "vk_loader.h"
#include "vk_pipelines.h"
#include "vk_types.h"

// Number of frames to overlap in flight
constexpr unsigned int FRAME_OVERLAP = 2;

// ---------------------------
// Structures and Helper Types
// ---------------------------

/**
 * Implements the PBR Metallic-Roughness workflow for materials
 */
struct GLTFMetallic_Roughness {
    MaterialPipeline opaquePipeline;
    MaterialPipeline transparentPipeline;

    VkDescriptorSetLayout materialLayout;

    // Material properties passed to shaders
    struct MaterialConstants {
        glm::vec4 colorFactors;
        glm::vec4 metal_rough_factors;
        // Padding for uniform buffer alignment
        glm::vec4 extra[14];
    };

    // Texture and buffer resources for a material
    struct MaterialResources {
        AllocatedImage colorImage;
        VkSampler colorSampler;
        AllocatedImage metalRoughImage;
        VkSampler metalRoughSampler;
        VkBuffer dataBuffer;
        uint32_t dataBufferOffset;
    };

    DescriptorWriter writer;

    void build_pipelines(class VulkanEngine* engine);
    void clear_resources(VkDevice device);

    MaterialInstance write_material(
            VkDevice device, MaterialPass pass,
            const MaterialResources& resources,
            DescriptorAllocatorGrowable& descriptorAllocator);
};

/**
 * Manages deferred resource cleanup in LIFO order
 */
struct DeletionQueue {
    std::deque<std::function<void()>> deletors;

    void push_function(std::function<void()>&& function) {
        deletors.push_back(function);
    }

    void flush() {
        // Execute all functions in reverse order
        for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
            (*it)();
        }
        deletors.clear();
    }
};

/**
 * Per-frame resources for triple buffering
 */
struct FrameData {
    VkCommandPool _commandPool;
    VkCommandBuffer _mainCommandBuffer;

    VkSemaphore _swapchainSemaphore, _renderSemaphore;
    VkFence _renderFence;

    DeletionQueue _deletionQueue;
    DescriptorAllocatorGrowable _frameDescriptors;
};

/**
 * Scene data passed to GPU through uniform buffer
 */
struct GPUSceneData {
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewproj;
    glm::vec4 ambientColor;
    glm::vec4 sunlightDirection;  // w component is sun power
    glm::vec4 sunlightColor;
};

/**
 * Scene node containing a mesh
 */
struct MeshNode : public ENode {
    std::shared_ptr<MeshAsset> mesh;

    virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx) override;
};

/**
 * Renderable object with all data needed for a draw call
 */
struct RenderObject {
    uint32_t indexCount;
    uint32_t firstIndex;
    VkBuffer indexBuffer;

    MaterialInstance* material;

    glm::mat4 transform;
    VkDeviceAddress vertexBufferAddress;
};

/**
 * Collects objects to be drawn in a frame
 */
struct DrawContext {
    std::vector<RenderObject> OpaqueSurfaces;
};

// ---------------------------
// VulkanEngine Class
// ---------------------------

/**
 * Main renderer class managing the Vulkan instance and rendering pipeline
 */
class VulkanEngine {
public:
    // Singleton accessor
    static VulkanEngine& Get();

    // Initialization/Cleanup
    void init(struct SDL_Window* window);
    void cleanup();

    // Main loop methods
    void update();
    void draw();

    // Mesh and scene management
    int64_t registerMesh(std::string filePath);
    void unregisterMesh(int64_t id);
    void setMeshTransform(int64_t id, glm::mat4 mat);
    void update_scene();

    // Get current frame data
    FrameData& get_current_frame() {
        return _frames[_frameNumber % FRAME_OVERLAP];
    }

    // Execute a command immediately
    void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

    // Upload mesh data to GPU
    GPUMeshBuffers uploadMesh(std::span<uint32_t> indices,
                              std::span<Vertex> vertices);

    // Image creation and management
    AllocatedImage create_image(VkExtent3D size, VkFormat format,
                                VkImageUsageFlags usage,
                                bool mipmapped = false) const;
    AllocatedImage create_image(void* data, VkExtent3D size, VkFormat format,
                                VkImageUsageFlags usage,
                                bool mipmapped = false);
    void destroy_image(const AllocatedImage& img) const;

    // Create GPU buffer
    AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage,
                                  VmaMemoryUsage memoryUsage) const;

    // -- Public Fields --

    // Asset management
    std::unordered_map<int64_t, std::shared_ptr<LoadedGLTF>> meshes;
    std::unordered_map<int64_t, glm::mat4> transforms;
    std::unordered_map<std::string, std::shared_ptr<LoadedGLTF>> loadedScenes;

    // Scene management
    Camera* mainCamera;
    DrawContext mainDrawContext;
    std::unordered_map<std::string, std::shared_ptr<ENode>> loadedNodes;

    // Frame management
    FrameData _frames[FRAME_OVERLAP];

    // Vulkan queues
    VkQueue _graphicsQueue;
    uint32_t _graphicsQueueFamily;

    // Engine state
    bool _isInitialized{false};
    int _frameNumber{0};
    bool stop_rendering{false};
    VkExtent2D _windowExtent{2560, 1440};

    // Window and Vulkan instance
    struct SDL_Window* _window{nullptr};

    VkInstance _instance;                       // Vulkan library handle
    VkDebugUtilsMessengerEXT _debug_messenger;  // Debug output handle
    VkPhysicalDevice _chosenGPU;                // Selected GPU device
    VkDevice _device;                           // Logical device for commands
    VkSurfaceKHR _surface;                      // Window surface

    // Swapchain resources
    VkSwapchainKHR _swapchain;
    VkFormat _swapchainImageFormat;
    std::vector<VkImage> _swapchainImages;
    std::vector<VkImageView> _swapchainImageViews;
    VkExtent2D _swapchainExtent;

    // Resource management
    DeletionQueue _mainDeletionQueue;
    VmaAllocator _allocator;

    // Render targets
    AllocatedImage _drawImage;
    AllocatedImage _depthImage;
    VkExtent2D _drawExtent;
    float renderScale = 1.f;

    // Descriptor resources
    DescriptorAllocatorGrowable globalDescriptorAllocator;

    VkDescriptorSet _drawImageDescriptors;
    VkDescriptorSetLayout _drawImageDescriptorLayout;

    // Pipeline resources
    VkPipeline _gradientPipeline;
    VkPipelineLayout _gradientPipelineLayout;

    // Immediate submission resources
    VkFence _immFence;
    VkCommandBuffer _immCommandBuffer;
    VkCommandPool _immCommandPool;

    // Pipeline resources
    VkPipelineLayout _trianglePipelineLayout;
    VkPipeline _trianglePipeline;
    VkPipelineLayout _meshPipelineLayout;
    VkPipeline _meshPipeline;

    GPUMeshBuffers rectangle;

    std::vector<std::shared_ptr<MeshAsset>> testMeshes;

    bool resize_requested;
    GPUSceneData sceneData;

    VkDescriptorSetLayout _gpuSceneDataDescriptorLayout;

    // Default texture resources
    AllocatedImage _whiteImage;
    AllocatedImage _blackImage;
    AllocatedImage _greyImage;
    AllocatedImage _errorCheckerboardImage;

    // Default samplers
    VkSampler _defaultSamplerLinear;
    VkSampler _defaultSamplerNearest;

    VkDescriptorSetLayout _singleImageDescriptorLayout;

    // Default material
    MaterialInstance defaultData;
    GLTFMetallic_Roughness metalRoughMaterial;

private:
    // Debug callback for validation layers
    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                  void* pUserData);

    // Initialization methods
    void init_vulkan();
    void init_swapchain();
    void init_commands();
    void init_sync_structures();

    // Swapchain management
    void create_swapchain(uint32_t width, uint32_t height);
    void destroy_swapchain();

    // Rendering methods
    void draw_background(VkCommandBuffer cmd);
    void init_descriptors();
    void init_pipelines();
    void init_background_pipelines();
    void init_imgui();
    void init_triangle_pipeline();
    void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView) const;
    void draw_geometry(VkCommandBuffer cmd);
    void destroy_buffer(const AllocatedBuffer& buffer) const;
    void resize_swapchain();
    void init_mesh_pipeline();
    void init_default_data();
};