#pragma once

#include <cstddef>
#include <cstdint>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float4.hpp>
#include <memory>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>
#include <vk_mem_alloc.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include "vk_descriptors.h"
#include "vk_types.h"
#include "vk_smart_wrappers.h"

#include "pipelines.h"
#include "ComputePipeline.h"

#include "vk_command_buffers.h"
#include "vk_command_buffers_container.h"

class Camera;
class VulkanEngine;
struct DrawContext;
struct LoadedGLTF;
struct MeshAsset;


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

    Pipelines pipelines;

    CommandBuffers command_buffers;
    CommandBuffersContainer command_buffers_container;

    int64_t registerMesh(const std::string& filePath);

    void unregisterMesh(int64_t id);

    void setMeshTransform(int64_t id, glm::mat4 mat);

    std::unordered_map<int64_t, std::shared_ptr<LoadedGLTF>> meshes;

    std::unordered_map<int64_t, glm::mat4> transforms;

    std::unordered_map<std::string, std::shared_ptr<LoadedGLTF>> loadedScenes;

    Camera* mainCamera;

    DrawContext mainDrawContext;
    std::unordered_map<std::string, std::shared_ptr<ENode>> loadedNodes;

    void update_scene();

    FrameData& get_current_frame() {
        return command_buffers_container.get_current_frame(_frameNumber);
    };

    VkQueue _graphicsQueue;
    uint32_t _graphicsQueueFamily;

    bool _isInitialized{false};
    unsigned int _frameNumber{0};
    bool stop_rendering{false};
    VkExtent2D _windowExtent{2560, 1440};

    struct SDL_Window* _window{nullptr};

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
    VkPhysicalDevice _chosenGPU;  // GPU chosen as the default device
    VkDevice _device;             // Vulkan device for commands
    VkSurfaceKHR _surface;        // Vulkan window surface

    VkSwapchainKHR _swapchain;
    VkFormat _swapchainImageFormat;

    std::vector<VkImage> _swapchainImages;
    std::vector<VkImageView> _swapchainImageViews;
    VkExtent2D _swapchainExtent;

    VmaAllocator _allocator;

    std::unique_ptr<VulkanImage> _drawImage;
    std::unique_ptr<VulkanImage> _depthImage;
    VkExtent2D _drawExtent;
    float renderScale = 1.f;

    DescriptorAllocatorGrowable globalDescriptorAllocator;

    VkDescriptorSet _drawImageDescriptors;
    VkDescriptorSetLayout _drawImageDescriptorLayout;


    GPUMeshBuffers rectangle;

    GPUMeshBuffers uploadMesh(std::span<uint32_t> indices,
                              std::span<Vertex> vertices);

    std::vector<std::shared_ptr<MeshAsset>> testMeshes;

    bool resize_requested;

    GPUSceneData sceneData;

    VkDescriptorSetLayout _gpuSceneDataDescriptorLayout;

    AllocatedImage create_image(VkExtent3D size, VkFormat format,
                                VkImageUsageFlags usage,
                                bool mipmapped = false) const;
    AllocatedImage create_image(const void* data, VkExtent3D size,
                                VkFormat format, VkImageUsageFlags usage,
                                bool mipmapped = false) const;
    void destroy_image(const AllocatedImage& img) const;

    std::unique_ptr<VulkanImage> _whiteImage;
    std::unique_ptr<VulkanImage> _blackImage;
    std::unique_ptr<VulkanImage> _greyImage;
    std::unique_ptr<VulkanImage> _errorCheckerboardImage;

    VkSampler _defaultSamplerLinear;
    VkSampler _defaultSamplerNearest;

    VkDescriptorSetLayout _singleImageDescriptorLayout;

    MaterialInstance defaultData;
    GLTFMetallic_Roughness metalRoughMaterial;

    AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage,
                                  VmaMemoryUsage memoryUsage) const;

private:
    // Smart pointer collections for automatic cleanup
    std::vector<std::unique_ptr<VulkanBuffer>> _managedBuffers;
    std::vector<std::unique_ptr<VulkanImage>> _managedImages;

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                  void* pUserData);

    void init_vulkan();
    void init_swapchain();

    void create_swapchain(uint32_t width, uint32_t height);
    void destroy_swapchain();

    void draw_background(VkCommandBuffer cmd) const;

    void init_descriptors();

    void init_pipelines();
    void init_imgui();

    void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView) const;

    void draw_geometry(VkCommandBuffer cmd);

    void destroy_buffer(const AllocatedBuffer& buffer) const;

    void resize_swapchain();

    void init_mesh_pipeline();

    void init_default_data();
};
