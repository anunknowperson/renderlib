#include "internal/vulkan_render.h"

#include "../core/logging.h"

std::vector<glm::mat4> VulkanRender::instances = {};

void VulkanRender::init(GLFWwindow* p_window)
{

    // Identity matrix for no transformation
    glm::mat4 mat = glm::translate(glm::mat4(1.0f), glm::vec3(1000.0f, 1000.0f, 1000.0f));

    instances.push_back(mat);




    //

    lastTime = glfwGetTime();
    nbFrames = 0;

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

    createDescriptorSetLayout();

    vulkan_graphics_pipeline = std::make_shared<VulkanGraphicsPipeline>(this);

    vulkan_swap_chain->create_framebuffers();

    vulkan_command_pool = std::make_shared<VulkanCommandPool>(this);

    createVertexBuffer();
    createIndexBuffer();
    createInstancesBuffer();


    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();

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


    updateUniformBuffer(currentFrame);
    updateInstanceBuffer();

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



    double currentTime = glfwGetTime();





    nbFrames++;
    if ( currentTime - lastTime >= 1.0 ){
        LOGIF("{} ms ({} FPS)", 1000.0/double(nbFrames), double(nbFrames));
        nbFrames = 0;
        lastTime = currentTime;
    }


    drawFrame();
    vkDeviceWaitIdle(vulkan_device->device);

}

VulkanRender::VulkanRender()
{

}

VulkanRender::~VulkanRender()
{

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(vulkan_device->device, uniformBuffers[i], nullptr);
        vkFreeMemory(vulkan_device->device, uniformBuffersMemory[i], nullptr);
    }

    vkDestroyDescriptorPool(vulkan_device->device, descriptorPool, nullptr);

    vkDestroyDescriptorSetLayout(vulkan_device->device, descriptorSetLayout, nullptr);

    vkDestroyBuffer(vulkan_device->device, instanceBuffer, nullptr);
    vkFreeMemory(vulkan_device->device, instanceBufferMemory, nullptr);

    vkDestroyBuffer(vulkan_device->device, indexBuffer, nullptr);
    vkFreeMemory(vulkan_device->device, indexBufferMemory, nullptr);

    vkDestroyBuffer(vulkan_device->device, vertexBuffer, nullptr);
    vkFreeMemory(vulkan_device->device, vertexBufferMemory, nullptr);

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

void VulkanRender::createVertexBuffer() {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(vulkan_device->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(vulkan_device->device, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

    copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(vulkan_device->device, stagingBuffer, nullptr);
    vkFreeMemory(vulkan_device->device, stagingBufferMemory, nullptr);
}

uint32_t VulkanRender::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vulkan_physical_device, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    LOGE("Failed to find suitable memory type!");

    return -1;
}

void VulkanRender::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                                VkBuffer &buffer, VkDeviceMemory &bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vulkan_device->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        LOGE("Failed to create buffer.");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vulkan_device->device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(vulkan_device->device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        LOGE("Failed to allocate buffer memory.");
    }

    vkBindBufferMemory(vulkan_device->device, buffer, bufferMemory, 0);

}

void VulkanRender::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = vulkan_command_pool->commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(vulkan_device->device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(vulkan_device->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(vulkan_device->graphicsQueue);

    vkFreeCommandBuffers(vulkan_device->device, vulkan_command_pool->commandPool, 1, &commandBuffer);
}

void VulkanRender::createIndexBuffer() {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(vulkan_device->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t) bufferSize);
    vkUnmapMemory(vulkan_device->device, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

    copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(vulkan_device->device, stagingBuffer, nullptr);
    vkFreeMemory(vulkan_device->device, stagingBufferMemory, nullptr);
}

void VulkanRender::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;

    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(vulkan_device->device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        LOGE("Failed to create descriptor set layout.");
    }


}

void VulkanRender::createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

        vkMapMemory(vulkan_device->device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
    }
}

void VulkanRender::updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    ubo.proj = glm::perspective(glm::radians(45.0f), (float) vulkan_swap_chain->swapChainExtent.width / (float) vulkan_swap_chain->swapChainExtent.height, 0.1f, 10.0f);

    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void VulkanRender::createDescriptorPool() {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;

    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(vulkan_device->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        LOGE("Failed to create descriptor pool.");
    }
}

void VulkanRender::createDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(vulkan_device->device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        LOGE("Failed to allocate descriptor sets.");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(vulkan_device->device, 1, &descriptorWrite, 0, nullptr);
    }


}

void VulkanRender::createInstancesBuffer() {
    VkDeviceSize bufferSize = sizeof(instances[0]) * instances.size();

    currentBufferSize = bufferSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(vulkan_device->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, instances.data(), (size_t) bufferSize);
    vkUnmapMemory(vulkan_device->device, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, instanceBuffer, instanceBufferMemory);

    copyBuffer(stagingBuffer, instanceBuffer, bufferSize);

    vkDestroyBuffer(vulkan_device->device, stagingBuffer, nullptr);
    vkFreeMemory(vulkan_device->device, stagingBufferMemory, nullptr);
}

void VulkanRender::updateInstanceBuffer() {
    VkDeviceSize newBufferSize = sizeof(instances[0]) * instances.size();

    // Check if the buffer needs to be resized
    if (newBufferSize > currentBufferSize) {
        // Destroy the old buffer and create a new one with larger size
        vkDestroyBuffer(vulkan_device->device, instanceBuffer, nullptr);
        vkFreeMemory(vulkan_device->device, instanceBufferMemory, nullptr);

        createBuffer(newBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, instanceBuffer, instanceBufferMemory);
        currentBufferSize = newBufferSize;
    }

    // Use a staging buffer for the new data
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(newBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(vulkan_device->device, stagingBufferMemory, 0, newBufferSize, 0, &data);
    memcpy(data, instances.data(), (size_t) newBufferSize);
    vkUnmapMemory(vulkan_device->device, stagingBufferMemory);

    // Copy from staging buffer to the instance buffer
    copyBuffer(stagingBuffer, instanceBuffer, newBufferSize);

    // Clean up staging buffer
    vkDestroyBuffer(vulkan_device->device, stagingBuffer, nullptr);
    vkFreeMemory(vulkan_device->device, stagingBufferMemory, nullptr);
}
