#pragma once
#include "vulkr_device.hpp"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <memory>

namespace vulkr {
    class VulkrSwapChain {
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        VulkrSwapChain(VulkrDevice &deviceRef, VkExtent2D windowExtent);

        VulkrSwapChain(VulkrDevice &deviceRef, VkExtent2D windowExtent, std::shared_ptr<VulkrSwapChain> previous);

        ~VulkrSwapChain();

        VulkrSwapChain(const VulkrSwapChain &) = delete;

        void operator=(const VulkrSwapChain &) = delete;

        VkFramebuffer getFrameBuffer(const int index) const { return swapChainFramebuffers[index]; }
        VkRenderPass getRenderPass() const { return renderPass; }
        VkImageView getImageView(const int index) const { return swapChainImageViews[index]; }
        size_t imageCount() const { return swapChainImages.size(); }
        VkFormat getSwapChainImageFormat() const { return swapChainImageFormat; }
        VkExtent2D getSwapChainExtent() const { return swapChainExtent; }
        uint32_t width() const { return swapChainExtent.width; }
        uint32_t height() const { return swapChainExtent.height; }

        float extentAspectRatio() const {
            return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
        }

        VkFormat findDepthFormat();

        VkResult acquireNextImage(uint32_t *imageIndex);

        VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

        bool compareSwapFormats(const VulkrSwapChain &swapChain) const {
            return swapChain.swapChainImageFormat == swapChainImageFormat &&
                   swapChain.swapChainDepthFormat == swapChainDepthFormat;
        }

    private:
        void init();

        void createSwapChain();

        void createImageViews();

        void createDepthResources();

        void createRenderPass();

        void createFramebuffers();

        void createSyncObjects();

        // Helper functions
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR> &availableFormats);

        VkPresentModeKHR chooseSwapPresentMode(
            const std::vector<VkPresentModeKHR> &availablePresentModes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        VkFormat swapChainImageFormat;
        VkFormat swapChainDepthFormat;
        VkExtent2D swapChainExtent;

        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkRenderPass renderPass;

        std::vector<VkImage> depthImages;
        std::vector<VkDeviceMemory> depthImageMemorys;
        std::vector<VkImageView> depthImageViews;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;

        VulkrDevice &device;
        VkExtent2D windowExtent;

        VkSwapchainKHR swapChain;
        std::shared_ptr<VulkrSwapChain> oldSwapChain;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame = 0;
    };
}
