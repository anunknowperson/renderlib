#include "internal/vulkan_render.h"

#include "core/logging.h"


void VulkanRender::init(GLFWwindow* p_window)
{
	window = p_window;

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

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
    vkWaitForFences(vulkan_device->device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;

    VkResult result = vkAcquireNextImageKHR(vulkan_device->device, vulkan_swap_chain->swap_chain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        vulkan_swap_chain->recreate();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        LOGE("Failed to acquire swap chain image.");
    }

    vkResetFences(vulkan_device->device, 1, &inFlightFences[currentFrame]);

    vkResetCommandBuffer(vulkan_command_pool->commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    vulkan_command_pool->recordCommandBuffer(vulkan_command_pool->commandBuffers[currentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vulkan_command_pool->commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(vulkan_device->graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
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

    result = vkQueuePresentKHR(vulkan_device->presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        vulkan_swap_chain->recreate();
    } else if (result != VK_SUCCESS) {
        LOGE("Failed to present swap chain image.");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
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

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(vulkan_device->device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(vulkan_device->device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(vulkan_device->device, inFlightFences[i], nullptr);
    }

}

void VulkanRender::createSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(vulkan_device->device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(vulkan_device->device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(vulkan_device->device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

            LOGE("Failed to create synchronization objects for a frame.");
        }
    }
}

void VulkanRender::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto app = reinterpret_cast<VulkanRender*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;


}
