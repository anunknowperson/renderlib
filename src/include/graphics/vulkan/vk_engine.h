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

#include "vk_descriptors.h"
#include "vk_types.h"

class Camera;
class VulkanEngine;
struct DrawContext;
struct LoadedGLTF;
struct MeshAsset;

constexpr unsigned int frameOverlap = 2;

struct GLTFMetallicRoughness {
    MaterialPipeline opaque_pipeline;
    MaterialPipeline transparent_pipeline;

    VkDescriptorSetLayout material_layout;

    struct MaterialConstants {
        glm::vec4 color_factors;
        glm::vec4 metal_rough_factors;
        // padding, we need it anyway for uniform buffers
        glm::vec4 extra[14];
    };

    struct MaterialResources {
        AllocatedImage color_image;
        VkSampler color_sampler;
        AllocatedImage metal_rough_image;
        VkSampler metal_rough_sampler;
        VkBuffer data_buffer;
        uint32_t data_buffer_offset;
    };

    DescriptorWriter writer;

    void buildPipelines(VulkanEngine* engine);
    void clearResources(VkDevice device);

    MaterialInstance writeMaterial(
            VkDevice device, MaterialPass pass,
            const MaterialResources& resources,
            DescriptorAllocatorGrowable& descriptor_allocator);
};

struct DeletionQueue {
    std::deque<std::function<void()>> deletors;

    void pushFunction(std::function<void()>&& function) {
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
    VkCommandPool command_pool;
    VkCommandBuffer main_command_buffer;

    VkSemaphore swapchain_semaphore, render_semaphore;
    VkFence render_fence;

    DeletionQueue deletion_queue;
    DescriptorAllocatorGrowable frame_descriptors;
};

struct GPUSceneData {
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewproj;
    glm::vec4 ambient_color;
    glm::vec4 sunlight_direction;  // w for sun power
    glm::vec4 sunlight_color;
};

struct MeshNode : public ENode {
    MeshNode() = default;

    std::shared_ptr<MeshAsset> mesh;

    void draw(const glm::mat4& top_matrix, DrawContext& ctx) override;
};

struct RenderObject {
    uint32_t index_count;
    uint32_t first_index;
    VkBuffer index_buffer;

    MaterialInstance* material;

    glm::mat4 transform;
    VkDeviceAddress vertex_buffer_address;
};

struct DrawContext {
    std::vector<RenderObject> opaque_surfaces;
};

class VulkanEngine {
public:
    int64_t registerMesh(const std::string& file_path);
    void unregisterMesh(int64_t id);

    void setMeshTransform(int64_t id, glm::mat4 mat);

    std::unordered_map<int64_t, std::shared_ptr<LoadedGLTF>> meshes;

    std::unordered_map<int64_t, glm::mat4> transforms;

    std::unordered_map<std::string, std::shared_ptr<LoadedGLTF>> loaded_scenes;

    Camera* main_camera;

    DrawContext main_draw_context;
    std::unordered_map<std::string, std::shared_ptr<ENode>> loaded_nodes;

    void updateScene();

    FrameData frames[frameOverlap];

    FrameData& getCurrentFrame() {
        return frames[frame_number % frameOverlap];
    };

    VkQueue graphics_queue;
    uint32_t graphics_queue_family;

    bool is_initialized{false};
    unsigned int frame_number{0};
    bool stop_rendering{false};
    VkExtent2D window_extent{2560, 1440};

    struct SDL_Window* _window{nullptr};

    static VulkanEngine& get();

    // initializes everything in the engine
    void init(struct SDL_Window* window);

    // shuts down the engine
    void cleanup();

    // draw loop
    void draw();

    // run main loop
    void update();

    VkInstance instance;                       // Vulkan library handle
    VkDebugUtilsMessengerEXT debug_messenger;  // Vulkan debug output handle
    VkPhysicalDevice chosenGPU;  // GPU chosen as the default device
    VkDevice device;             // Vulkan device for commands
    VkSurfaceKHR surface;        // Vulkan window surface

    VkSwapchainKHR swapchain;
    VkFormat swapchain_image_format;

    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_image_views;
    VkExtent2D swapchain_extent;

    DeletionQueue main_deletion_queue;

    VmaAllocator allocator;

    AllocatedImage draw_image;
    AllocatedImage depth_image;
    VkExtent2D draw_extent;
    float render_scale = 1.f;

    DescriptorAllocatorGrowable global_descriptor_allocator;

    VkDescriptorSet draw_image_descriptors;
    VkDescriptorSetLayout draw_image_descriptor_layout;

    VkPipeline gradient_pipeline;
    VkPipelineLayout gradient_pipeline_layout;

    // immediate submit structures
    VkFence imm_fence;
    VkCommandBuffer imm_command_buffer;
    VkCommandPool imm_command_pool;

    VkPipelineLayout triangle_pipeline_layout;
    VkPipeline triangle_pipeline;

    VkPipelineLayout mesh_pipeline_layout;
    VkPipeline mesh_pipeline;

    GPUMeshBuffers rectangle;

    void immediateSubmit(
            std::function<void(VkCommandBuffer cmd)>&& function) const;
    GPUMeshBuffers uploadMesh(std::span<uint32_t> indices,
                              std::span<Vertex> vertices);

    std::vector<std::shared_ptr<MeshAsset>> test_meshes;

    bool resize_requested;

    GPUSceneData scene_data;

    VkDescriptorSetLayout gpu_scene_data_descriptor_layout;

    AllocatedImage createImage(VkExtent3D size, VkFormat format,
                                VkImageUsageFlags usage,
                                bool mipmapped = false) const;
    AllocatedImage createImage(const void* data, VkExtent3D size,
                                VkFormat format, VkImageUsageFlags usage,
                                bool mipmapped = false) const;
    void destroyImage(const AllocatedImage& img) const;

    AllocatedImage white_image;
    AllocatedImage black_image;
    AllocatedImage grey_image;
    AllocatedImage error_checkerboard_image;

    VkSampler default_sampler_linear;
    VkSampler default_sampler_nearest;

    VkDescriptorSetLayout single_image_descriptor_layout;

    MaterialInstance default_data;
    GLTFMetallicRoughness metal_rough_material;

    AllocatedBuffer createBuffer(size_t alloc_size, VkBufferUsageFlags usage,
                                  VmaMemoryUsage memory_usage) const;

private:
    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                  VkDebugUtilsMessageTypeFlagsEXT message_type,
                  const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
                  void* p_user_data);

    void initVulkan();
    void initSwapchain();
    void initCommands();
    void initSyncStructures();

    void createSwapchain(uint32_t width, uint32_t height);
    void destroySwapchain();

    void drawBackground(VkCommandBuffer cmd) const;

    void initDescriptors();

    void initPipelines();
    void initBackgroundPipelines();
    void initImgui();

    void initTrianglePipeline();

    void drawImgui(VkCommandBuffer cmd, VkImageView target_image_view) const;

    void drawGeometry(VkCommandBuffer cmd);

    void destroyBuffer(const AllocatedBuffer& buffer) const;

    void resizeSwapchain();

    void initMeshPipeline();

    void initDefaultData();
};