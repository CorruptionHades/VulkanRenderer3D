//
// Created by corru on 20/07/2025.
//

#ifndef VULKR_RENDERER_H
#define VULKR_RENDERER_H
#include <cassert>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "../pipeline/vulkr_device.hpp"
#include "../pipeline/vulkr_swap_chain.hpp"


namespace vulkr {
    class VulkrRenderer {
    public:
        VulkrRenderer(VulkrWindow &window, VulkrDevice &device);

        ~VulkrRenderer();

        VulkrRenderer(const VulkrRenderer &) = delete;

        VulkrRenderer &operator=(const VulkrRenderer &) = delete;

        VkRenderPass getSwapChainRenderPass() const { return vulkrSwapChain->getRenderPass(); }
        float getAspectRatio() const { return vulkrSwapChain->extentAspectRatio(); }

        [[nodiscard]] bool isFrameInProgress() const { return isFrameStarted; }

        VkCommandBuffer getCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer when frame is not in progress!");
            return commandBuffers[currentFrameIndex];
        }

        int getFrameIndex() const {
            assert(isFrameStarted && "Cannot get frame index when frame is not in progress!");
            return currentFrameIndex;
        }

        VkCommandBuffer beginFrame();

        void endFrame();

        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);

        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void createCommandBuffers();

        void freeCommandBuffers();

        void recreateSwapChain();

        VulkrWindow &vulkrWindow;
        VulkrDevice &vulkrDevice;

        std::unique_ptr<VulkrSwapChain> vulkrSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex{0};
        int currentFrameIndex{0};
        bool isFrameStarted{false};
    };
}

#endif //VULKR_RENDERER_H
