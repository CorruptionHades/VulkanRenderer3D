//
// Created by CorruptionHades on 22/07/2025.
//

#ifndef HUD_RENDER_SYSTEM_H
#define HUD_RENDER_SYSTEM_H
#include <array>
#include <memory>

#include "../../pipeline/vulkr_device.hpp"
#include "../../pipeline/vulkr_pipeline.h"
#include "../../model/vulkr_model.h"

namespace vulkr {
    class HudRenderSystem {
    public:
        HudRenderSystem(VulkrDevice &device, VkRenderPass renderPass);

        ~HudRenderSystem();

        void render(VkCommandBuffer commandBuffer, float aspect);

        void renderNumber(VkCommandBuffer commandBuffer, int number, float x, float y, float scale, float aspect);

    private:
        void createPipelineLayout();

        void createPipeline(VkRenderPass renderPass);

        VulkrDevice &vulkrDevice;
        std::unique_ptr<VulkrPipeline> hudPipeline;

        VkPipelineLayout pipelineLayout;

        std::array<std::unique_ptr<VulkrModel>, 10> digitModels;
        std::unique_ptr<VulkrModel> crosshairModel;
    };
}


#endif //HUD_RENDER_SYSTEM_H
