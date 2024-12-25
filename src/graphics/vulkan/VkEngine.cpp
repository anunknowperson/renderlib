#include "graphics/vulkan/VkEngine.h"

#include "core/config.h"

#define VMA_IMPLEMENTATION
#include "SDL_vulkan.h"
#include "VkBootstrap.h"
#include "graphics/vulkan/VkImages.h"
#include "graphics/vulkan/VkInitializers.h"
#include "graphics/vulkan/VkLoader.h"
#include "graphics/vulkan/VkPipelines.h"
#include "graphics/vulkan/VkTypes.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"
#include "vk_mem_alloc.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"

VulkanEngine* loaded_engine = nullptr;

VulkanEngine& VulkanEngine::get() {
    return *loaded_engine;
}

#ifdef NDEBUG
constexpr bool bUseValidationLayers = false;
#else
constexpr bool bUseValidationLayers = true;
#endif

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanEngine::debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
        [[maybe_unused]] void* pUserData) {
    std::string type;

    switch (messageType) {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
            type = "General";

            break;

        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
            type = "Validation";

            break;

        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
            type = "Performance";

            break;
    }

    std::string message = "(" + type + ")" + p_callback_data->pMessage;

    if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        LOGD(message)
    } else if (message_severity ==
               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        LOGI(message)
    } else if (message_severity ==
               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        LOGW(message)
    } else if (message_severity >=
               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        LOGE("{}", message)
    }

    return VK_FALSE;
}

void VulkanEngine::initDefaultData() {
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

    auto path_to_assets = std::string(ASSETS_DIR) + "/basicmesh.glb";
    test_meshes = loadGltfMeshes(this, path_to_assets).value();

    // 3 default textures, white, grey, black. 1 pixel each
    uint32_t white = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
    white_image =
            createImage((void*)&white, VkExtent3D{1, 1, 1},
                        VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

    uint32_t grey = glm::packUnorm4x8(glm::vec4(0.66f, 0.66f, 0.66f, 1));
    grey_image =
            createImage((void*)&grey, VkExtent3D{1, 1, 1},
                        VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

    uint32_t black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 0));
    black_image =
            createImage((void*)&black, VkExtent3D{1, 1, 1},
                        VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

    // checkerboard image
    uint32_t magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
    std::array<uint32_t, 16 * 16> pixels{};  // for 16x16 checkerboard texture
    for (size_t x = 0; x < 16; x++) {
        for (size_t y = 0; y < 16; y++) {
            pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
        }
    }
    error_checkerboard_image =
            createImage(pixels.data(), VkExtent3D{16, 16, 1},
                        VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

    VkSamplerCreateInfo sampl = {.sType =
                                         VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};

    sampl.magFilter = VK_FILTER_NEAREST;
    sampl.minFilter = VK_FILTER_NEAREST;

    vkCreateSampler(device, &sampl, nullptr, &default_sampler_nearest);

    sampl.magFilter = VK_FILTER_LINEAR;
    sampl.minFilter = VK_FILTER_LINEAR;
    vkCreateSampler(device, &sampl, nullptr, &default_sampler_linear);

    GLTFMetallicRoughness::MaterialResources material_resources{};
    // default the material textures
    material_resources.color_image = white_image;
    material_resources.color_sampler = default_sampler_linear;
    material_resources.metal_rough_image = white_image;
    material_resources.metal_rough_sampler = default_sampler_linear;

    // set the uniform buffer for the material data
    AllocatedBuffer material_constants = createBuffer(
            sizeof(GLTFMetallicRoughness::MaterialConstants),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

    // write the buffer
    auto* scene_uniform_data =
            (GLTFMetallicRoughness::MaterialConstants*)
                    material_constants.allocation->GetMappedData();
    scene_uniform_data->color_factors = glm::vec4{1, 1, 1, 1};
    scene_uniform_data->metal_rough_factors = glm::vec4{1, 0.5, 0, 0};

    main_deletion_queue.pushFunction(
            [=, this]() { destroyBuffer(material_constants); });

    material_resources.data_buffer = material_constants.buffer;
    material_resources.data_buffer_offset = 0;

    default_data = metal_rough_material.writeMaterial(
            device, MaterialPass::MainColor, material_resources,
            global_descriptor_allocator);
}

void VulkanEngine::initMeshPipeline() {
    VkShaderModule triangle_frag_shader;
    if (!vkutil::loadShaderModule("./shaders/tex_image.frag.spv", device,
                                  &triangle_frag_shader)) {
        fmt::println("Error when building the triangle fragment shader module");
    } else {
        fmt::println("Triangle fragment shader successfully loaded");
    }

    VkShaderModule triangle_vertex_shader;
    if (!vkutil::loadShaderModule("./shaders/colored_triangle_mesh.vert.spv",
                                  device, &triangle_vertex_shader)) {
        fmt::println("Error when building the triangle vertex shader module");
    } else {
        fmt::println("Triangle vertex shader successfully loaded");
    }

    VkPushConstantRange buffer_range{};
    buffer_range.offset = 0;
    buffer_range.size = sizeof(GPUDrawPushConstants);
    buffer_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineLayoutCreateInfo pipeline_layout_info =
            vkinit::pipelineLayoutCreateInfo();
    pipeline_layout_info.pPushConstantRanges = &buffer_range;
    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pSetLayouts = &single_image_descriptor_layout;
    pipeline_layout_info.setLayoutCount = 1;
    VK_CHECK(vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr,
                                    &mesh_pipeline_layout));

    PipelineBuilder pipeline_builder;

    // use the triangle layout we created
    pipeline_builder.pipeline_layout = mesh_pipeline_layout;
    // connecting the vertex and pixel shaders to the pipeline
    pipeline_builder.setShaders(triangle_vertex_shader, triangle_frag_shader);
    // it will draw triangles
    pipeline_builder.setInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    // filled triangles
    pipeline_builder.setPolygonMode(VK_POLYGON_MODE_FILL);
    // no backface culling
    pipeline_builder.setCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
    // no multisampling
    pipeline_builder.setMultisamplingNone();
    // no blending
    pipeline_builder.disableBlending();

    // pipeline_builder.disableDepthtest();
    pipeline_builder.enableDepthtest(true, VK_COMPARE_OP_GREATER);

    // connect the image format we will draw into, from draw image
    pipeline_builder.setColorAttachmentFormat(draw_image.image_format);
    pipeline_builder.setDepthFormat(VK_FORMAT_UNDEFINED);

    // finally build the pipeline
    mesh_pipeline = pipeline_builder.buildPipeline(device);

    // clean structures
    vkDestroyShaderModule(device, triangle_frag_shader, nullptr);
    vkDestroyShaderModule(device, triangle_vertex_shader, nullptr);

    main_deletion_queue.pushFunction([&]() {
        vkDestroyPipelineLayout(device, mesh_pipeline_layout, nullptr);
        vkDestroyPipeline(device, mesh_pipeline, nullptr);
    });
}

void VulkanEngine::initTrianglePipeline() {
    VkShaderModule triangle_frag_shader;
    if (!vkutil::loadShaderModule("./shaders/colored_triangle.frag.spv", device,
                                  &triangle_frag_shader)) {
        fmt::println("Error when building the triangle fragment shader module");
    } else {
        fmt::println("Triangle fragment shader successfully loaded");
    }

    VkShaderModule triangle_vertex_shader;
    if (!vkutil::loadShaderModule("./shaders/colored_triangle.vert.spv", device,
                                  &triangle_vertex_shader)) {
        fmt::println("Error when building the triangle vertex shader module");
    } else {
        fmt::println("Triangle vertex shader successfully loaded");
    }

    // build the pipeline layout that controls the inputs/outputs of the shader
    // we are not using descriptor sets or other systems yet, so no need to use
    // anything other than empty default
    VkPipelineLayoutCreateInfo pipeline_layout_info =
            vkinit::pipelineLayoutCreateInfo();
    VK_CHECK(vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr,
                                    &triangle_pipeline_layout));

    PipelineBuilder pipeline_builder;

    // use the triangle layout we created
    pipeline_builder.pipeline_layout = triangle_pipeline_layout;
    // connecting the vertex and pixel shaders to the pipeline
    pipeline_builder.setShaders(triangle_vertex_shader, triangle_frag_shader);
    // it will draw triangles
    pipeline_builder.setInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    // filled triangles
    pipeline_builder.setPolygonMode(VK_POLYGON_MODE_FILL);
    // no backface culling
    pipeline_builder.setCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
    // no multisampling
    pipeline_builder.setMultisamplingNone();
    // no blending
    pipeline_builder.disableBlending();
    // no depth testing
    pipeline_builder.disableDepthtest();

    // connect the image format we will draw into, from draw image
    pipeline_builder.setColorAttachmentFormat(draw_image.image_format);
    pipeline_builder.setDepthFormat(VK_FORMAT_UNDEFINED);

    // finally build the pipeline
    triangle_pipeline = pipeline_builder.buildPipeline(device);

    // clean structures
    vkDestroyShaderModule(device, triangle_frag_shader, nullptr);
    vkDestroyShaderModule(device, triangle_vertex_shader, nullptr);

    main_deletion_queue.pushFunction([&]() {
        vkDestroyPipelineLayout(device, triangle_pipeline_layout, nullptr);
        vkDestroyPipeline(device, triangle_pipeline, nullptr);
    });
}

void VulkanEngine::initImgui() {
    // 1: create descriptor pool for IMGUI
    //  the size of the pool is very oversize, but it's copied from imgui demo
    //  itself.
    VkDescriptorPoolSize pool_sizes[] = {
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

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    VkDescriptorPool imgui_pool;
    VK_CHECK(vkCreateDescriptorPool(device, &pool_info, nullptr, &imgui_pool));

    // 2: initialize imgui library

    // this initializes the core structures of imgui
    ImGui::CreateContext();

    // this initializes imgui for SDL
    ImGui_ImplSDL2_InitForVulkan(window);

    // this initializes imgui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = instance;
    init_info.PhysicalDevice = chosenGPU;
    init_info.Device = device;
    init_info.Queue = graphics_queue;
    init_info.DescriptorPool = imgui_pool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.UseDynamicRendering = true;

    // dynamic rendering parameters for imgui to use
    init_info.PipelineRenderingCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
    init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats =
            &swapchain_image_format;

    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info);

    ImGui_ImplVulkan_CreateFontsTexture();

    // add destroy the imgui created structures
    main_deletion_queue.pushFunction([=, this]() {
        ImGui_ImplVulkan_Shutdown();
        vkDestroyDescriptorPool(device, imgui_pool, nullptr);
    });
}

void VulkanEngine::initDescriptors() {
    // create a descriptor pool that will hold 10 sets with 1 image each
    std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> sizes = {
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}};

    global_descriptor_allocator.init(device, 10, sizes);

    // make the descriptor set layout for our compute draw
    {
        DescriptorLayoutBuilder builder;
        builder.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
        draw_image_descriptor_layout =
                builder.build(device, VK_SHADER_STAGE_COMPUTE_BIT);
    }

    {
        DescriptorLayoutBuilder builder;
        builder.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        gpu_scene_data_descriptor_layout =
                builder.build(device, VK_SHADER_STAGE_VERTEX_BIT |
                                               VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    {
        DescriptorLayoutBuilder builder;
        builder.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        single_image_descriptor_layout =
                builder.build(device, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    // allocate a descriptor set for our draw image
    draw_image_descriptors = global_descriptor_allocator.allocate(
            device, draw_image_descriptor_layout);

    DescriptorWriter writer;
    writer.writeImage(0, draw_image.image_view, VK_NULL_HANDLE,
                       VK_IMAGE_LAYOUT_GENERAL,
                       VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);

    writer.updateSet(device, draw_image_descriptors);

    for (auto & frame : frames) {
        // create a descriptor pool
        std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> frame_sizes = {
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4},
        };

        frame.frame_descriptors = DescriptorAllocatorGrowable{};
        frame.frame_descriptors.init(device, 1000, frame_sizes);

        main_deletion_queue.pushFunction(
                [&]() { frame.frame_descriptors.destroyPools(device); });
    }
}

void VulkanEngine::initBackgroundPipelines() {
    VkPipelineLayoutCreateInfo compute_layout{};
    compute_layout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    compute_layout.pNext = nullptr;
    compute_layout.pSetLayouts = &draw_image_descriptor_layout;
    compute_layout.setLayoutCount = 1;

    VK_CHECK(vkCreatePipelineLayout(device, &compute_layout, nullptr,
                                    &gradient_pipeline_layout));

    VkShaderModule compute_draw_shader;
    if (!vkutil::loadShaderModule("./shaders/gradient.comp.spv", device,
                                  &compute_draw_shader)) {
        fmt::println("Error when building the compute shader \n");
    }

    VkPipelineShaderStageCreateInfo stage_info{};
    stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage_info.pNext = nullptr;
    stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    stage_info.module = compute_draw_shader;
    stage_info.pName = "main";

    VkComputePipelineCreateInfo compute_pipeline_create_info{};
    compute_pipeline_create_info.sType =
            VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    compute_pipeline_create_info.pNext = nullptr;
    compute_pipeline_create_info.layout = gradient_pipeline_layout;
    compute_pipeline_create_info.stage = stage_info;

    VK_CHECK(vkCreateComputePipelines(device, VK_NULL_HANDLE, 1,
                                      &compute_pipeline_create_info, nullptr,
                                      &gradient_pipeline));

    vkDestroyShaderModule(device, compute_draw_shader, nullptr);

    main_deletion_queue.pushFunction([&]() {
        vkDestroyPipelineLayout(device, gradient_pipeline_layout, nullptr);
        vkDestroyPipeline(device, gradient_pipeline, nullptr);
    });
}

void VulkanEngine::initPipelines() {
    initBackgroundPipelines();

    initTrianglePipeline();
    initMeshPipeline();

    metal_rough_material.buildPipelines(this);
}

void VulkanEngine::init(struct SDL_Window* _window) {
    _window = _window;

    // only one engine initialization is allowed with the application.
    assert(loaded_engine == nullptr);
    loaded_engine = this;

    initVulkan();
    initSwapchain();
    initCommands();
    initSyncStructures();
    initDescriptors();
    initPipelines();
    initImgui();
    initDefaultData();

    main_camera->velocity = glm::vec3(0.f);
    main_camera->position = glm::vec3(0, 0, 5);

    main_camera->pitch = 0;
    main_camera->yaw = 0;

    std::string structure_path = {std::string(ASSETS_DIR) + "/basicmesh.glb"};
    auto structure_file = loadGltf(this, structure_path);

    assert(structure_file.has_value());
    loaded_scenes["structure"] = *structure_file;

    is_initialized = true;
}

void VulkanEngine::initVulkan() {
    auto system_info_ret = vkb::SystemInfo::get_system_info();
    if (!system_info_ret) {
        LOGE("Failed to retrieve system info. Error: {}",
             system_info_ret.error().message());
    }

    auto system_info = system_info_ret.value();

    LOGI("Available layers:")

    for (auto& layer : system_info.available_layers) {
        LOGI(layer.layerName);
    }

    LOGI("Available extensions:")

    for (auto& extension : system_info.available_extensions) {
        LOGI(extension.extensionName);
    }

    vkb::InstanceBuilder builder;

    auto inst_ret = builder.set_app_name("TODO: PUT APP NAME HERE")
                            .set_engine_name("rainsystem")
                            .request_validation_layers(bUseValidationLayers)
                            .set_debug_callback(debugCallback)
                            .require_api_version(1, 3, 0)
                            .build();

    if (!inst_ret) {
        LOGE("Failed to create Vulkan instance. Error: {}",
             inst_ret.error().message());
    }

    vkb::Instance vkb_inst = inst_ret.value();

    // grab the instance
    instance = vkb_inst.instance;
    debug_messenger = vkb_inst.debug_messenger;

    SDL_bool err = SDL_Vulkan_CreateSurface(window, instance, &surface);
    if (!err) {
        LOGE("Failed to create Vulkan surface. Error: {}", SDL_GetError());
    }

    // vulkan 1.3 features
    VkPhysicalDeviceVulkan13Features features{};
    features.dynamicRendering = true;
    features.synchronization2 = true;

    // vulkan 1.2 features
    VkPhysicalDeviceVulkan12Features features12{};
    features12.bufferDeviceAddress = true;
    features12.descriptorIndexing = true;

    // use vkbootstrap to select a gpu.
    // We want a gpu that can write to the SDL surface and supports vulkan 1.3
    // with the correct features
    vkb::PhysicalDeviceSelector selector{vkb_inst};

    auto physical_device_ret = selector.set_minimum_version(1, 3)
                                       .set_required_features_13(features)
                                       .set_required_features_12(features12)
                                       .set_surface(surface)
                                       .select();

    if (!physical_device_ret) {
        LOGE("Failed to select physical device. Error: {}",
             physical_device_ret.error().message());
    }

    vkb::PhysicalDevice physical_device = physical_device_ret.value();

    vkb::DeviceBuilder device_builder{physical_device};

    auto dev_ret = device_builder.build();
    if (!dev_ret) {
        LOGE("Failed to create logical device. Error: {}",
             dev_ret.error().message());
    }

    vkb::Device vkb_device = dev_ret.value();

    // get the VkDevice handle used in the rest of a vulkan application
    device = vkb_device.device;
    chosenGPU = physical_device.physical_device;

    auto queue_ret = vkb_device.get_queue(vkb::QueueType::graphics);
    if (!queue_ret) {
        LOGE("Failed to retrieve graphics queue. Error: {}",
             queue_ret.error().message());
    }

    graphics_queue = queue_ret.value();

    auto queue_family_ret =
            vkb_device.get_queue_index(vkb::QueueType::graphics);
    if (!queue_family_ret) {
        LOGE("Failed to retrieve graphics queue family. Error: {}",
             queue_family_ret.error().message());
    }

    graphics_queue_family = queue_family_ret.value();

    // initialize the memory allocator
    VmaAllocatorCreateInfo allocator_info = {};
    allocator_info.physicalDevice = chosenGPU;
    allocator_info.device = device;
    allocator_info.instance = instance;
    allocator_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    vmaCreateAllocator(&allocator_info, &allocator);

    main_deletion_queue.pushFunction([&]() { vmaDestroyAllocator(allocator); });
}

void VulkanEngine::initCommands() {
    // create a command pool for commands submitted to the graphics queue.
    // we also want the pool to allow for resetting of individual command
    // buffers
    VkCommandPoolCreateInfo command_pool_info = vkinit::commandPoolCreateInfo(
            graphics_queue_family,
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    for (auto & _frame : frames) {
        VK_CHECK(vkCreateCommandPool(device, &command_pool_info, nullptr,
                                     &_frame.command_pool));

        // allocate the default command buffer that we will use for rendering
        VkCommandBufferAllocateInfo cmd_alloc_info =
                vkinit::commandBufferAllocateInfo(_frame.command_pool,
                                                     1);

        VK_CHECK(vkAllocateCommandBuffers(device, &cmd_alloc_info,
                                          &_frame.main_command_buffer));
    }

    VK_CHECK(vkCreateCommandPool(device, &command_pool_info, nullptr,
                                 &imm_command_pool));

    // allocate the command buffer for immediate submits
    VkCommandBufferAllocateInfo cmd_alloc_info =
            vkinit::commandBufferAllocateInfo(imm_command_pool, 1);

    VK_CHECK(vkAllocateCommandBuffers(device, &cmd_alloc_info,
                                      &imm_command_buffer));

    main_deletion_queue.pushFunction([=, this]() {
        vkDestroyCommandPool(device, imm_command_pool, nullptr);
    });
}

void VulkanEngine::initSyncStructures() {
    VkFenceCreateInfo fence_create_info =
            vkinit::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
    VkSemaphoreCreateInfo semaphore_create_info = vkinit::semaphoreCreateInfo();

    for (auto & _frame : frames) {
        VK_CHECK(vkCreateFence(device, &fence_create_info, nullptr,
                               &_frame.render_fence));

        VK_CHECK(vkCreateSemaphore(device, &semaphore_create_info, nullptr,
                                   &_frame.swapchain_semaphore));
        VK_CHECK(vkCreateSemaphore(device, &semaphore_create_info, nullptr,
                                   &_frame.render_semaphore));
    }

    VK_CHECK(vkCreateFence(device, &fence_create_info, nullptr, &imm_fence));
    main_deletion_queue.pushFunction(
            [=, this]() { vkDestroyFence(device, imm_fence, nullptr); });
}

void VulkanEngine::createSwapchain(uint32_t width, uint32_t height) {
    vkb::SwapchainBuilder swapchain_builder{chosenGPU, device, surface};

    swapchain_image_format = VK_FORMAT_B8G8R8A8_UNORM;

    auto swap_ret =
            swapchain_builder
                    //.use_default_format_selection()
                    .set_desired_format(VkSurfaceFormatKHR{
                            .format = swapchain_image_format,
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

    vkb::Swapchain vkb_swapchain = swap_ret.value();

    swapchain_extent = vkb_swapchain.extent;
    // store swapchain and its related images
    swapchain = vkb_swapchain.swapchain;
    swapchain_images = vkb_swapchain.get_images().value();
    swapchain_image_views = vkb_swapchain.get_image_views().value();
}

void VulkanEngine::initSwapchain() {
    createSwapchain(window_extent.width, window_extent.height);

    // draw image size will match the window
    VkExtent3D draw_image_extent = {window_extent.width, window_extent.height, 1};

    // hardcoding the draw format to 32-bit float
    draw_image.image_format = VK_FORMAT_R16G16B16A16_SFLOAT;
    draw_image.image_extent = draw_image_extent;

    VkImageUsageFlags draw_image_usages{};
    draw_image_usages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    draw_image_usages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    draw_image_usages |= VK_IMAGE_USAGE_STORAGE_BIT;
    draw_image_usages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkImageCreateInfo rimg_info = vkinit::imageCreateInfo(
            draw_image.image_format, draw_image_usages, draw_image_extent);

    // for the draw image, we want to allocate it from gpu local memory
    VmaAllocationCreateInfo rimg_alloc_info = {};
    rimg_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    rimg_alloc_info.requiredFlags =
            VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // allocate and create the image
    vmaCreateImage(allocator, &rimg_info, &rimg_alloc_info, &draw_image.image,
                   &draw_image.allocation, nullptr);

    // build an image-view for the draw image to use for rendering
    VkImageViewCreateInfo rview_info = vkinit::imageviewCreateInfo(
            draw_image.image_format, draw_image.image,
            VK_IMAGE_ASPECT_COLOR_BIT);

    VK_CHECK(vkCreateImageView(device, &rview_info, nullptr,
                               &draw_image.image_view));

    // add to deletion queues
    main_deletion_queue.pushFunction([=, this]() {
        vkDestroyImageView(device, draw_image.image_view, nullptr);
        vmaDestroyImage(allocator, draw_image.image, draw_image.allocation);
    });
}

void VulkanEngine::destroySwapchain() {
    vkDestroySwapchainKHR(device, swapchain, nullptr);

    // destroy swapchain resources
    for (auto & swapchain_image_view : swapchain_image_views) {
        vkDestroyImageView(device, swapchain_image_view, nullptr);
    }
}

void VulkanEngine::cleanup() {
    if (is_initialized) {
        // make sure the gpu has stopped doing its things
        vkDeviceWaitIdle(device);

        loaded_scenes.clear();

        main_deletion_queue.flush();

        for (auto & _frame : frames) {
            // already written from before
            vkDestroyCommandPool(device, _frame.command_pool, nullptr);

            // destroy sync objects
            vkDestroyFence(device, _frame.render_fence, nullptr);
            vkDestroySemaphore(device, _frame.render_semaphore, nullptr);
            vkDestroySemaphore(device, _frame.swapchain_semaphore,
                               nullptr);
        }

        destroySwapchain();

        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyDevice(device, nullptr);

        vkb::destroy_debug_utils_messenger(instance, debug_messenger);
        vkDestroyInstance(instance, nullptr);
    }

    // clear engine pointer
    loaded_engine = nullptr;
}

AllocatedBuffer VulkanEngine::createBuffer(size_t allocSize,
                                            VkBufferUsageFlags usage,
                                            VmaMemoryUsage memoryUsage) const {
    // allocate buffer
    VkBufferCreateInfo buffer_info = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    buffer_info.pNext = nullptr;
    buffer_info.size = allocSize;

    buffer_info.usage = usage;

    VmaAllocationCreateInfo vma_alloc_info = {};
    vma_alloc_info.usage = memoryUsage;
    vma_alloc_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    AllocatedBuffer new_buffer{};

    // allocate the buffer
    VK_CHECK(vmaCreateBuffer(allocator, &buffer_info, &vma_alloc_info,
                             &new_buffer.buffer, &new_buffer.allocation,
                             &new_buffer.info));

    return new_buffer;
}

void VulkanEngine::destroyBuffer(const AllocatedBuffer& buffer) const {
    vmaDestroyBuffer(allocator, buffer.buffer, buffer.allocation);
}

GPUMeshBuffers VulkanEngine::uploadMesh(std::span<uint32_t> indices,
                                        std::span<Vertex> vertices) {
    const size_t vertexBufferSize = vertices.size() * sizeof(Vertex);
    const size_t indexBufferSize = indices.size() * sizeof(uint32_t);

    GPUMeshBuffers new_surface{};

    // create vertex buffer
    new_surface.vertex_buffer =
            createBuffer(vertexBufferSize,
                         VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                 VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                 VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                         VMA_MEMORY_USAGE_GPU_ONLY);

    // find the address of the vertex buffer
    VkBufferDeviceAddressInfo device_address_info{
            .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
            .buffer = new_surface.vertex_buffer.buffer};
    new_surface.vertex_buffer_address =
            vkGetBufferDeviceAddress(device, &device_address_info);

    // create index buffer
    new_surface.index_buffer = createBuffer(
            indexBufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY);

    AllocatedBuffer staging = createBuffer(vertexBufferSize + indexBufferSize,
                                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                           VMA_MEMORY_USAGE_CPU_ONLY);

    void* data = staging.allocation->GetMappedData();

    // copy vertex buffer
    memcpy(data, vertices.data(), vertexBufferSize);
    // copy index buffer
    memcpy((char*)data + vertexBufferSize, indices.data(), indexBufferSize);

    immediateSubmit([&](VkCommandBuffer cmd) {
        VkBufferCopy vertex_copy{0};
        vertex_copy.dstOffset = 0;
        vertex_copy.srcOffset = 0;
        vertex_copy.size = vertexBufferSize;

        vkCmdCopyBuffer(cmd, staging.buffer, new_surface.vertex_buffer.buffer, 1,
                        &vertex_copy);

        VkBufferCopy index_copy{0};
        index_copy.dstOffset = 0;
        index_copy.srcOffset = vertexBufferSize;
        index_copy.size = indexBufferSize;

        vkCmdCopyBuffer(cmd, staging.buffer, new_surface.index_buffer.buffer, 1,
                        &index_copy);
    });

    destroyBuffer(staging);

    return new_surface;
}

void VulkanEngine::drawBackground(VkCommandBuffer cmd) {
    // bind the gradient drawing compute pipeline
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, gradient_pipeline);

    // bind the descriptor set containing the draw image for the compute
    // pipeline
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE,
                            gradient_pipeline_layout, 0, 1,
                            &draw_image_descriptors, 0, nullptr);

    // execute the compute pipeline dispatch. We are using 16x16 workgroup size,
    // so we need to divide by it
    vkCmdDispatch(cmd, static_cast<uint32_t>(std::ceil(draw_extent.width / 16.0)),
                  static_cast<uint32_t>(std::ceil(draw_extent.height / 16.0)), 1);
}

void VulkanEngine::drawImgui(VkCommandBuffer cmd,
                              VkImageView targetImageView) const {
    VkRenderingAttachmentInfo color_attachment = vkinit::attachmentInfo(
            targetImageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkRenderingInfo render_info =
            vkinit::renderingInfo(swapchain_extent, &color_attachment, nullptr);

    vkCmdBeginRendering(cmd, &render_info);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

    vkCmdEndRendering(cmd);
}

void VulkanEngine::drawGeometry(VkCommandBuffer cmd) {
    // allocate a new uniform buffer for the scene data
    AllocatedBuffer gpu_scene_data_buffer = createBuffer(
            sizeof(GPUSceneData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU);

    // add it to the deletion queue of this frame, so it gets deleted once it's
    // been used
    getCurrentFrame().deletion_queue.pushFunction(
            [=, this]() { destroyBuffer(gpu_scene_data_buffer); });

    // write the buffer
    auto* scene_uniform_data =
            (GPUSceneData*)gpu_scene_data_buffer.allocation->GetMappedData();
    *scene_uniform_data = scene_data;

    // create a descriptor set that binds that buffer and update it
    VkDescriptorSet global_descriptor =
            getCurrentFrame().frame_descriptors.allocate(
                    device, gpu_scene_data_descriptor_layout);

    DescriptorWriter writer;
    writer.writeBuffer(0, gpu_scene_data_buffer.buffer, sizeof(GPUSceneData), 0,
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    writer.updateSet(device, global_descriptor);

    VkRenderingAttachmentInfo color_attachment = vkinit::attachmentInfo(
            draw_image.image_view, nullptr, VK_IMAGE_LAYOUT_GENERAL);

    VkRenderingInfo render_info =
            vkinit::renderingInfo(draw_extent, &color_attachment, nullptr);
    vkCmdBeginRendering(cmd, &render_info);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, triangle_pipeline);

    // set dynamic viewport and scissor
    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<float>(draw_extent.width);
    viewport.height = static_cast<float>(draw_extent.height);
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = draw_extent.width;
    scissor.extent.height = draw_extent.height;

    vkCmdSetScissor(cmd, 0, 1, &scissor);

    // bind a texture
    VkDescriptorSet image_set = getCurrentFrame().frame_descriptors.allocate(
            device, single_image_descriptor_layout);
    {
        writer.writeImage(0, error_checkerboard_image.image_view,
                          default_sampler_nearest,
                           VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                           VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

        writer.updateSet(device, image_set);
    }

    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            mesh_pipeline_layout, 0, 1, &image_set, 0, nullptr);

    for (const RenderObject& draw : main_draw_context.opaque_surfaces) {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          draw.material->pipeline->pipeline);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                draw.material->pipeline->layout, 0, 1,
                                &global_descriptor, 0, nullptr);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                draw.material->pipeline->layout, 1, 1,
                                &draw.material->material_set, 0, nullptr);

        vkCmdBindIndexBuffer(cmd, draw.index_buffer, 0, VK_INDEX_TYPE_UINT32);

        GPUDrawPushConstants push_constants{};
        push_constants.vertex_buffer = draw.vertex_buffer_address;
        push_constants.world_matrix = draw.transform;
        vkCmdPushConstants(cmd, draw.material->pipeline->layout,
                           VK_SHADER_STAGE_VERTEX_BIT, 0,
                           sizeof(GPUDrawPushConstants), &push_constants);

        vkCmdDrawIndexed(cmd, draw.index_count, 1, draw.first_index, 0, 0);
    }

    vkCmdEndRendering(cmd);
}

void VulkanEngine::draw() {
    updateScene();

    // wait until the gpu has finished rendering the last frame. Timeout of 1
    // second
    VK_CHECK(vkWaitForFences(device, 1, &getCurrentFrame().render_fence,
                             true, 1000000000));

    getCurrentFrame().deletion_queue.flush();
    getCurrentFrame().frame_descriptors.clearPools(device);

    VK_CHECK(vkResetFences(device, 1, &getCurrentFrame().render_fence));

    // request image from the swapchain
    uint32_t swapchain_image_index;
    VkResult e = vkAcquireNextImageKHR(device, swapchain, 1000000000,
                                       getCurrentFrame().swapchain_semaphore,
                                       nullptr, &swapchain_image_index);
    if (e == VK_ERROR_OUT_OF_DATE_KHR) {
        resize_requested = true;
        return;
    }

    // naming it cmd for shorter writing
    VkCommandBuffer cmd = getCurrentFrame().main_command_buffer;

    // now that we are sure that the commands finished executing, we can safely
    // reset the command buffer to begin recording again.
    VK_CHECK(vkResetCommandBuffer(cmd, 0));

    // begin the command buffer recording. We will use this command buffer
    // exactly once, so we want to let vulkan know that
    VkCommandBufferBeginInfo cmd_begin_info = vkinit::commandBufferBeginInfo(
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    draw_extent.height = static_cast<uint32_t>(
            (float)std::min(swapchain_extent.height,
                            draw_image.image_extent.height) *
            render_scale);
    draw_extent.width = static_cast<uint32_t>(
            (float)std::min(swapchain_extent.width,
                            draw_image.image_extent.width) *
            render_scale);

    VK_CHECK(vkBeginCommandBuffer(cmd, &cmd_begin_info));

    // transition our main draw image into general layout, so we can write into
    // it, we will overwrite it all, so we don't care about what was the older
    // layout
    vkutil::transitionImage(cmd, draw_image.image, VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_GENERAL);

    drawBackground(cmd);

    vkutil::transitionImage(cmd, draw_image.image, VK_IMAGE_LAYOUT_GENERAL,
                            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    drawGeometry(cmd);

    // transition the draw image and the swapchain image into their correct
    // transfer layouts
    vkutil::transitionImage(cmd, draw_image.image,
                            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    vkutil::transitionImage(cmd, swapchain_images[swapchain_image_index],
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // execute a copy from the draw image into the swapchain
    vkutil::copyImageToImage(cmd, draw_image.image,
                             swapchain_images[swapchain_image_index],
                             draw_extent, swapchain_extent);

    // set swapchain image layout to Present, so we can show it on the screen
    vkutil::transitionImage(cmd, swapchain_images[swapchain_image_index],
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    // draw imgui into the swapchain image
    drawImgui(cmd, swapchain_image_views[swapchain_image_index]);

    // set swapchain image layout to Present, so we can draw it
    // vkutil::transitionImage(cmd, swapchain_images[swapchain_image_index],
    // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    // finalize the command buffer (we can no longer add commands, but it can
    // now be executed)
    VK_CHECK(vkEndCommandBuffer(cmd));

    // prepare the submission to the queue.
    // we want to wait on the _presentSemaphore, as that semaphore is signaled
    // when the swapchain is ready we will signal the render_semaphore, to
    // signal that rendering has finished

    VkCommandBufferSubmitInfo cmd_info = vkinit::commandBufferSubmitInfo(cmd);

    VkSemaphoreSubmitInfo wait_info = vkinit::semaphoreSubmitInfo(
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
            getCurrentFrame().swapchain_semaphore);
    VkSemaphoreSubmitInfo signalInfo =
            vkinit::semaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                                        getCurrentFrame().render_semaphore);

    VkSubmitInfo2 submit =
            vkinit::submitInfo(&cmd_info, &signalInfo, &wait_info);

    // submit command buffer to the queue and execute it.
    //  render_fence will now block until the graphic commands finish execution
    VK_CHECK(vkQueueSubmit2(graphics_queue, 1, &submit,
                            getCurrentFrame().render_fence));

    // prepare present
    //  this will put the image we just rendered to into the visible window.
    //  we want to wait on the render_semaphore for that,
    //  as its necessary that drawing commands have finished before the image is
    //  displayed to the user
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pNext = nullptr;
    present_info.pSwapchains = &swapchain;
    present_info.swapchainCount = 1;

    present_info.pWaitSemaphores = &getCurrentFrame().render_semaphore;
    present_info.waitSemaphoreCount = 1;

    present_info.pImageIndices = &swapchain_image_index;

    VkResult present_result = vkQueuePresentKHR(graphics_queue, &present_info);
    if (present_result == VK_ERROR_OUT_OF_DATE_KHR) {
        resize_requested = true;
    }

    // increase the number of frames drawn
    frame_number++;
}

void VulkanEngine::resizeSwapchain() {
    vkDeviceWaitIdle(device);

    destroySwapchain();

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    window_extent.width = static_cast<uint32_t>(w);
    window_extent.height = static_cast<uint32_t>(h);

    createSwapchain(window_extent.width, window_extent.height);

    resize_requested = false;
}

void VulkanEngine::immediateSubmit(
        std::function<void(VkCommandBuffer cmd)>&& function) {
    VK_CHECK(vkResetFences(device, 1, &imm_fence));
    VK_CHECK(vkResetCommandBuffer(imm_command_buffer, 0));

    VkCommandBuffer cmd = imm_command_buffer;

    VkCommandBufferBeginInfo cmd_begin_info = vkinit::commandBufferBeginInfo(
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VK_CHECK(vkBeginCommandBuffer(cmd, &cmd_begin_info));

    function(cmd);

    VK_CHECK(vkEndCommandBuffer(cmd));

    VkCommandBufferSubmitInfo cmd_info = vkinit::commandBufferSubmitInfo(cmd);
    VkSubmitInfo2 submit = vkinit::submitInfo(&cmd_info, nullptr, nullptr);

    // submit command buffer to the queue and execute it.
    //  render_fence will now block until the graphic commands finish execution
    VK_CHECK(vkQueueSubmit2(graphics_queue, 1, &submit, imm_fence));

    VK_CHECK(vkWaitForFences(device, 1, &imm_fence, true, 9999999999));
}

void VulkanEngine::update() {
    if (resize_requested) {
        resizeSwapchain();
    }

    draw();
}

AllocatedImage VulkanEngine::createImage(VkExtent3D size, VkFormat format,
                                          VkImageUsageFlags usage,
                                          bool mipmapped) const {
    AllocatedImage new_image{};
    new_image.image_format = format;
    new_image.image_extent = size;

    VkImageCreateInfo img_info = vkinit::imageCreateInfo(format, usage, size);
    if (mipmapped) {
        img_info.mipLevels = static_cast<uint32_t>(std::floor(std::log2(
                                     std::max(size.width, size.height)))) +
                             1;
    }

    // always allocate images on dedicated GPU memory
    VmaAllocationCreateInfo alloc_info = {};
    alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    alloc_info.requiredFlags =
            VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // allocate and create the image
    VK_CHECK(vmaCreateImage(allocator, &img_info, &alloc_info, &new_image.image,
                            &new_image.allocation, nullptr));

    // if the format is a depth format, we will need to have it use the correct
    // aspect flag
    VkImageAspectFlags aspect_flag = VK_IMAGE_ASPECT_COLOR_BIT;
    if (format == VK_FORMAT_D32_SFLOAT) {
        aspect_flag = VK_IMAGE_ASPECT_DEPTH_BIT;
    }

    // build an image-view for the image
    VkImageViewCreateInfo view_info =
            vkinit::imageviewCreateInfo(format, new_image.image, aspect_flag);
    view_info.subresourceRange.levelCount = img_info.mipLevels;

    VK_CHECK(vkCreateImageView(device, &view_info, nullptr,
                               &new_image.image_view));

    return new_image;
}

AllocatedImage VulkanEngine::createImage(void* data, VkExtent3D size,
                                          VkFormat format,
                                          VkImageUsageFlags usage,
                                          bool mipmapped) {
    size_t data_size = size.depth * size.width * size.height * 4;
    AllocatedBuffer upload_buffer =
            createBuffer(data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VMA_MEMORY_USAGE_CPU_TO_GPU);

    memcpy(upload_buffer.info.pMappedData, data, data_size);

    AllocatedImage new_image =
            createImage(size, format,
                        usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                        mipmapped);

    immediateSubmit([&](VkCommandBuffer cmd) {
        vkutil::transitionImage(cmd, new_image.image, VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VkBufferImageCopy copy_region = {};
        copy_region.bufferOffset = 0;
        copy_region.bufferRowLength = 0;
        copy_region.bufferImageHeight = 0;

        copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy_region.imageSubresource.mipLevel = 0;
        copy_region.imageSubresource.baseArrayLayer = 0;
        copy_region.imageSubresource.layerCount = 1;
        copy_region.imageExtent = size;

        // copy the buffer into the image
        vkCmdCopyBufferToImage(cmd, upload_buffer.buffer, new_image.image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                               &copy_region);

        vkutil::transitionImage(cmd, new_image.image,
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    });

    destroyBuffer(upload_buffer);

    return new_image;
}

void VulkanEngine::destroyImage(const AllocatedImage& img) const {
    vkDestroyImageView(device, img.image_view, nullptr);
    vmaDestroyImage(allocator, img.image, img.allocation);
}

void GLTFMetallicRoughness::buildPipelines(VulkanEngine* engine) {
    VkShaderModule mesh_frag_shader;
    if (!vkutil::loadShaderModule("./shaders/mesh.frag.spv", engine->device,
                                  &mesh_frag_shader)) {
        fmt::println("Error when building the triangle fragment shader module");
    }

    VkShaderModule mesh_vertex_shader;
    if (!vkutil::loadShaderModule("./shaders/mesh.vert.spv", engine->device,
                                  &mesh_vertex_shader)) {
        fmt::println("Error when building the triangle vertex shader module");
    }

    VkPushConstantRange matrix_range{};
    matrix_range.offset = 0;
    matrix_range.size = sizeof(GPUDrawPushConstants);
    matrix_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    DescriptorLayoutBuilder layout_builder;
    layout_builder.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    layout_builder.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    layout_builder.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    material_layout = layout_builder.build(
            engine->device,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

    VkDescriptorSetLayout layouts[] = {engine->gpu_scene_data_descriptor_layout,
                                       material_layout};

    VkPipelineLayoutCreateInfo mesh_layout_info =
            vkinit::pipelineLayoutCreateInfo();
    mesh_layout_info.setLayoutCount = 2;
    mesh_layout_info.pSetLayouts = layouts;
    mesh_layout_info.pPushConstantRanges = &matrix_range;
    mesh_layout_info.pushConstantRangeCount = 1;

    VkPipelineLayout new_layout;
    VK_CHECK(vkCreatePipelineLayout(engine->device, &mesh_layout_info, nullptr,
                                    &new_layout));

    opaque_pipeline.layout = new_layout;
    transparent_pipeline.layout = new_layout;

    // build the stage-create-info for both vertex and fragment stages. This
    // lets the pipeline know the shader modules per stage
    PipelineBuilder pipeline_builder;
    pipeline_builder.setShaders(mesh_vertex_shader, mesh_frag_shader);
    pipeline_builder.setInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    pipeline_builder.setPolygonMode(VK_POLYGON_MODE_FILL);
    pipeline_builder.setCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
    pipeline_builder.setMultisamplingNone();
    pipeline_builder.disableBlending();
    pipeline_builder.enableDepthtest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);

    // render format
    pipeline_builder.setColorAttachmentFormat(engine->draw_image.image_format);
    pipeline_builder.setDepthFormat(engine->depth_image.image_format);

    // use the triangle layout we created
    pipeline_builder.pipeline_layout = new_layout;

    // finally build the pipeline
    opaque_pipeline.pipeline = pipeline_builder.buildPipeline(engine->device);

    // create the transparent variant
    pipeline_builder.enableBlendingAdditive();

    pipeline_builder.enableDepthtest(false, VK_COMPARE_OP_GREATER_OR_EQUAL);

    transparent_pipeline.pipeline =
            pipeline_builder.buildPipeline(engine->device);

    vkDestroyShaderModule(engine->device, mesh_frag_shader, nullptr);
    vkDestroyShaderModule(engine->device, mesh_vertex_shader, nullptr);
}

MaterialInstance GLTFMetallicRoughness::writeMaterial(
        VkDevice device, MaterialPass pass, const MaterialResources& resources,
        DescriptorAllocatorGrowable& descriptor_allocator) {
    MaterialInstance mat_data{};
    mat_data.passType = pass;
    if (pass == MaterialPass::Transparent) {
        mat_data.pipeline = &transparent_pipeline;
    } else {
        mat_data.pipeline = &opaque_pipeline;
    }

    mat_data.material_set =
            descriptor_allocator.allocate(device, material_layout);

    writer.clear();
    writer.writeBuffer(0, resources.data_buffer, sizeof(MaterialConstants),
                        resources.data_buffer_offset,
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    writer.writeImage(1, resources.color_image.image_view,
                       resources.color_sampler,
                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    writer.writeImage(2, resources.metal_rough_image.image_view,
                       resources.metal_rough_sampler,
                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    writer.updateSet(device, mat_data.material_set);
    writer.updateSet(device, mat_data.material_set);

    return mat_data;
}

void MeshNode::draw(const glm::mat4& top_matrix, DrawContext& ctx) {
    glm::mat4 node_matrix = top_matrix * world_transform;

    for (auto& s : mesh->surfaces) {
        RenderObject def{};
        def.index_count = s.count;
        def.first_index = s.start_index;
        def.index_buffer = mesh->mesh_buffers.index_buffer.buffer;
        def.material = &s.material->data;

        def.transform = node_matrix;
        def.vertex_buffer_address = mesh->mesh_buffers.vertex_buffer_address;

        ctx.opaque_surfaces.push_back(def);
    }

    ENode::draw(top_matrix, ctx);
}

void VulkanEngine::updateScene() {
    main_camera->update();

    glm::mat4 view = main_camera->getViewMatrix();

    glm::mat4 projection = glm::perspective(
            glm::radians(70.f),
            (float)window_extent.width / (float)window_extent.height, 0.1f,
            10000.f);

    // to opengl and gltf axis
    projection[1][1] *= -1;

    scene_data.view = view;
    scene_data.proj = projection;
    scene_data.viewproj = projection * view;

    main_draw_context.opaque_surfaces.clear();

    scene_data.ambient_color = glm::vec4(.1f);
    scene_data.sunlight_color = glm::vec4(1.f);
    scene_data.sunlight_direction = glm::vec4(0, 1, 0.5, 1.f);

    for (const auto& [key, mesh] : meshes) {
        std::shared_ptr<LoadedGLTF> loaded_mesh = mesh;
        loaded_mesh->draw(transforms[key], main_draw_context);
    }
}

int64_t VulkanEngine::registerMesh(std::string file_path) {
    std::random_device rd;

    // Use the Mersenne Twister engine for high-quality random numbers
    std::mt19937_64 generator(rd());

    // Create a uniform distribution for int64_t
    std::uniform_int_distribution<int64_t> distribution;

    // Generate and print a random int64_t value
    int64_t random_int64 = distribution(generator);

    std::string structure_path = {std::string(ASSETS_DIR) + file_path};
    auto structure_file = loadGltf(this, structure_path);

    assert(structure_file.has_value());

    meshes[random_int64] = *structure_file;
    transforms[random_int64] = glm::mat4(1.0f);

    return random_int64;
}

void VulkanEngine::unregisterMesh(int64_t id) {
    meshes.erase(id);
    transforms.erase(id);
}

void VulkanEngine::setMeshTransform(int64_t id, glm::mat4 mat) {
    transforms[id] = mat;
}
