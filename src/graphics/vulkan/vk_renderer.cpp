#include "graphics/vulkan/vk_renderer.h"
#include "graphics/vulkan/vk_engine.h"
#include "graphics/vulkan/vk_initializers.h"
#include "graphics/vulkan/vk_images.h"

#include <vk_mem_alloc.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>
#include <glm/glm.hpp>

void VulkanRenderer::init(VulkanEngine* engine) {
    _engine = engine;
}

void VulkanRenderer::draw_background(VkCommandBuffer cmd) const {
    // Bind the gradient drawing compute pipeline
    _engine->pipelines.gradientPipeline->bind(cmd);

    // Bind descriptor sets
    _engine->pipelines.gradientPipeline->bindDescriptorSets(cmd, &_engine->_drawImageDescriptors, 1);

    // Dispatch the compute shader
    _engine->pipelines.gradientPipeline->dispatch(cmd, 
        static_cast<uint32_t>(std::ceil(_engine->_drawExtent.width / 16.0)),
        static_cast<uint32_t>(std::ceil(_engine->_drawExtent.height / 16.0)), 
        1);
}

void VulkanRenderer::draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView) const {
    const VkRenderingAttachmentInfo colorAttachment = vkinit::attachment_info(
            targetImageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    const VkRenderingInfo renderInfo =
            vkinit::rendering_info(_engine->_swapchainExtent, &colorAttachment, nullptr);

    vkCmdBeginRendering(cmd, &renderInfo);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

    vkCmdEndRendering(cmd);
}

void VulkanRenderer::draw_geometry(VkCommandBuffer cmd) {
    // Allocate a new uniform buffer for the scene data
    AllocatedBuffer gpuSceneDataBuffer = _engine->create_buffer(
            sizeof(GPUSceneData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU);

    // Add it to the deletion queue of this frame, so it gets deleted once it's been used
    _engine->get_current_frame()._deletionQueue.push_function(
            [=, this] { _engine->destroy_buffer(gpuSceneDataBuffer); });

    void* mappedData;
    vmaMapMemory(_engine->_allocator, gpuSceneDataBuffer.allocation, &mappedData);
    GPUSceneData* sceneUniformData = static_cast<GPUSceneData*>(mappedData);
    *sceneUniformData = _engine->sceneData;
    vmaUnmapMemory(_engine->_allocator, gpuSceneDataBuffer.allocation);

    // Create a descriptor set that binds that buffer and update it
    VkDescriptorSet globalDescriptor =
            _engine->get_current_frame()._frameDescriptors.allocate(
                    _engine->_device, _engine->_gpuSceneDataDescriptorLayout);

    DescriptorWriter writer;
    writer.write_buffer(0, gpuSceneDataBuffer.buffer, sizeof(GPUSceneData), 0,
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    writer.update_set(_engine->_device, globalDescriptor);

    VkRenderingAttachmentInfo colorAttachment = vkinit::attachment_info(
            _engine->_drawImage.imageView, nullptr, VK_IMAGE_LAYOUT_GENERAL);

    VkRenderingInfo renderInfo =
            vkinit::rendering_info(_engine->_drawExtent, &colorAttachment, nullptr);
    vkCmdBeginRendering(cmd, &renderInfo);

    _engine->pipelines.trianglePipeline->bind(cmd);

    // Set dynamic viewport and scissor
    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<float>(_engine->_drawExtent.width);
    viewport.height = static_cast<float>(_engine->_drawExtent.height);
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = _engine->_drawExtent.width;
    scissor.extent.height = _engine->_drawExtent.height;

    vkCmdSetScissor(cmd, 0, 1, &scissor);

    // Bind a texture
    VkDescriptorSet imageSet = _engine->get_current_frame()._frameDescriptors.allocate(
            _engine->_device, _engine->_singleImageDescriptorLayout);
    DescriptorWriter single_image_writer;
    single_image_writer.write_image(0, _engine->_errorCheckerboardImage.imageView,
                                    _engine->_defaultSamplerNearest,
                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    single_image_writer.update_set(_engine->_device, imageSet);

    _engine->pipelines.meshPipeline->bindDescriptorSets(cmd, &imageSet, 1);

    for (const auto& [indexCount, firstIndex, indexBuffer, material, transform,
                      vertexBufferAddress] : _engine->mainDrawContext.OpaqueSurfaces) {
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

void VulkanRenderer::draw_frame(VkCommandBuffer cmd, VkImageView targetImageView, uint32_t swapchainImageIndex) {
    // Transition our main draw image into general layout
    vkutil::transition_image(cmd, _engine->_drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED,
                         VK_IMAGE_LAYOUT_GENERAL);

    // Draw the background
    draw_background(cmd);

    vkutil::transition_image(cmd, _engine->_drawImage.image, VK_IMAGE_LAYOUT_GENERAL,
                         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    // Draw geometry
    draw_geometry(cmd);

    // Transition the draw image and the swapchain image into their correct transfer layouts
    vkutil::transition_image(cmd, _engine->_drawImage.image,
                         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                         VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    vkutil::transition_image(cmd, _engine->_swapchainImages[swapchainImageIndex],
                         VK_IMAGE_LAYOUT_UNDEFINED,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // Execute a copy from the draw image into the swapchain
    vkutil::copy_image_to_image(cmd, _engine->_drawImage.image,
                            _engine->_swapchainImages[swapchainImageIndex],
                            _engine->_drawExtent, _engine->_swapchainExtent);

    // Set swapchain image layout to Present, so we can show it on the screen
    vkutil::transition_image(cmd, _engine->_swapchainImages[swapchainImageIndex],
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                         VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    // Draw imgui into the swapchain image
    draw_imgui(cmd, targetImageView);
}