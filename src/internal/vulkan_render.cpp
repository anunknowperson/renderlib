#include "internal/vulkan_render.h"

#include "core/logging.h"


void VulkanRender::init(GLFWwindow* p_window)
{
	window = p_window;


	vulkan_instance = std::make_shared<VulkanInstance>();


    vulkan_surface = std::make_shared<VulkanSurface>(this);

	vulkan_physical_device = VulkanPhysicalDevice::pick(this);

    vulkan_device = std::make_shared<VulkanDevice>(this);

    vulkan_swap_chain = std::make_shared<VulkanSwapchain>(this);

    vulkan_swap_chain->createImageViews();

    vulkan_render_pass = std::make_shared<VulkanRenderPass>(this);

    vulkan_graphics_pipeline = std::make_shared<VulkanGraphicsPipeline>(this);

    vulkan_swap_chain->create_framebuffers();

    vulkan_command_pool = std::make_shared<VulkanCommandPool>(this);

    createSyncObjects();
}

void VulkanRender::drawFrame() {
    vkWaitForFences(vulkan_device->device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

    vkResetFences(vulkan_device->device, 1, &inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(vulkan_device->device, vulkan_swap_chain->swap_chain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(vulkan_command_pool->commandBuffer, 0);

    vulkan_command_pool->recordCommandBuffer(vulkan_command_pool->commandBuffer, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vulkan_command_pool->commandBuffer;

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(vulkan_device->graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
        LOGE("Failed to submit draw command buffer.");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {vulkan_swap_chain->swap_chain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr; // Optional

    vkQueuePresentKHR(vulkan_device->presentQueue, &presentInfo);
}

void VulkanRender::render()
{


    drawFrame();
    vkDeviceWaitIdle(vulkan_device->device);

}

VulkanRender::VulkanRender()
{

}

VulkanRender::~VulkanRender()
{
    vkDestroySemaphore(vulkan_device->device, imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(vulkan_device->device, renderFinishedSemaphore, nullptr);
    vkDestroyFence(vulkan_device->device, inFlightFence, nullptr);
}

void VulkanRender::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(vulkan_device->device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(vulkan_device->device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(vulkan_device->device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
        LOGE("Failed to create semaphores.");
    }
}


