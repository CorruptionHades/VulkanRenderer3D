//
// Created by CorruptionHades on 20/07/2025.
//

#ifndef BONE_RENDER_SYSTEM_H
#define BONE_RENDER_SYSTEM_H

#include "../pipeline/vulkr_device.hpp"
#include "../pipeline/vulkr_pipeline.h"
#include "../model/vulkr_model.h"
#include "../game/camera.h"

namespace vulkr {
    class BoneRenderSystem {
    public:
        BoneRenderSystem(VulkrDevice &device, VkRenderPass renderPass);

        ~BoneRenderSystem();

        BoneRenderSystem(const BoneRenderSystem &) = delete;

        BoneRenderSystem &operator=(const BoneRenderSystem &) = delete;

        void renderBones(VkCommandBuffer commandBuffer, VulkrModel &model, const Camera &camera);

    private:
        void createPipelineLayout();

        void createPipeline(VkRenderPass renderPass);

        VulkrDevice &vulkrDevice;
        std::unique_ptr<VulkrPipeline> vulkrPipeline;
        VkPipelineLayout pipelineLayout;
    };
}

#endif //BONE_RENDER_SYSTEM_H
