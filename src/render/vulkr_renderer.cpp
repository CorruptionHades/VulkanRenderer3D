//
// Created by CorruptionHades on 20/07/2025.
//

#include "vulkr_renderer.h"

#include <array>
#include <functional>
#include <stdexcept>
#include <chrono>

namespace vulkr {

    VulkrRenderer::VulkrRenderer(VulkrWindow &window, VulkrDevice &device)
    : vulkrWindow(window), vulkrDevice(device), isFrameStarted(false), currentFrameIndex(0) {
        recreateSwapChain();
        createCommandBuffers();
    }

    VulkrRenderer::~VulkrRenderer() {
        freeCommandBuffers();
    }

    VkCommandBuffer VulkrRenderer::beginFrame() {
        assert(!isFrameStarted && "Cannot call beginFrame while a frame is already in progress!");

        auto result = vulkrSwapChain->acquireNextImage(&currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to acquire swap chain image!");
        }

        isFrameStarted = true;

        const auto commandBuffer = getCurrentCommandBuffer();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin command buffer operation!");
        }

        return commandBuffer;
    }

    void VulkrRenderer::endFrame() {
        assert (isFrameStarted && "Cannot call endFrame while a frame is not in progress!");
        const auto commandBuffer = getCurrentCommandBuffer();

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to end command buffer operation!");
        }

        auto result = vulkrSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vulkrWindow.wasFrameBufferResized()) {
            vulkrWindow.resetFrameBufferResized();
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to submit command buffer commands!");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % VulkrSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void VulkrRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "Cannot call beginSwapChainRenderPass when frame is not in progress!");
        assert(commandBuffer == getCurrentCommandBuffer() && "Cannot call beginSwapChainRenderPass with command buffer that is not current!");

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin command buffer operation!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = vulkrSwapChain->getRenderPass();
        renderPassInfo.framebuffer = vulkrSwapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset.x = 0;
        renderPassInfo.renderArea.offset.y = 0;
        renderPassInfo.renderArea.extent = vulkrSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.4f, 0.4f, 0.4f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(vulkrSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(vulkrSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, vulkrSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void VulkrRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "Cannot call endSwapChainRenderPass when frame is not in progress!");
        assert(commandBuffer == getCurrentCommandBuffer() && "Cannot call endSwapChainRenderPass with command buffer that is not current!");

        vkCmdEndRenderPass(commandBuffer);
    }

    void VulkrRenderer::createCommandBuffers() {
        commandBuffers.resize(VulkrSwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = vulkrDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(vulkrDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void VulkrRenderer::freeCommandBuffers() {
        vkFreeCommandBuffers(vulkrDevice.device(), vulkrDevice.getCommandPool(), commandBuffers.size(), commandBuffers.data());
        commandBuffers.clear();
    }

    void VulkrRenderer::recreateSwapChain() {
        auto extent = vulkrWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = vulkrWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(vulkrDevice.device());

        if (vulkrSwapChain == nullptr) {
            vulkrSwapChain = std::make_unique<VulkrSwapChain>(vulkrDevice, extent);
        }
        else {
            std::shared_ptr<VulkrSwapChain> oldSwapChain = std::move(vulkrSwapChain);
            vulkrSwapChain = std::make_unique<VulkrSwapChain>(vulkrDevice, extent, oldSwapChain);

            if (!oldSwapChain->compareSwapFormats(*vulkrSwapChain)) {
                throw std::runtime_error("swap chain image (or depth) format has changed!");
            }
        }
    }
}

