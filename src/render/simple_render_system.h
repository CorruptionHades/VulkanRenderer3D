//
// Created by CorruptionHades on 20/07/2025.
//

#ifndef SIMPLE_RENDER_SYSTEM_H
#define SIMPLE_RENDER_SYSTEM_H

#include "../game/camera.h"
#include "../game/game_object.h"
#include "../pipeline/vulkr_device.hpp"
#include "../pipeline/vulkr_pipeline.h"

namespace vulkr {
    class VulkrPipeline;

    class SimpleRenderSystem {
    public:
        SimpleRenderSystem(VulkrDevice &device, VkRenderPass renderPass);

        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem &) = delete;

        SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

        void renderGameObjects(VkCommandBuffer command_buffer, std::vector<GameObject> &game_objects,
                               const Camera &camera);

    private:
        void createPipelineLayout();

        void createPipeline(VkRenderPass renderPass);

        VulkrDevice &vulkrDevice;

        std::unique_ptr<VulkrPipeline> vulkrPipeline;
        VkPipelineLayout pipelineLayout;

        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorPool descriptorPool;
    };
}


#endif //SIMPLE_RENDER_SYSTEM_H
