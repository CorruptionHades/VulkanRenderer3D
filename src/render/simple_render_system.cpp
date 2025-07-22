//
// Created by CorruptionHades on 20/07/2025.
//

#include "simple_render_system.h"
#include <glm/gtc/constants.hpp>

namespace vulkr {
    struct SimplePushConstantData {
        glm::mat4 transform{1.0f}; // Identity matrix (no transformation)
        glm::mat4 normalMatrix{1.0f};
        int enableLighting{1}; // 1 to enable lighting, 0 to disable
    };

    SimpleRenderSystem::SimpleRenderSystem(VulkrDevice &device, VkRenderPass renderPass)
        : vulkrDevice(device) {
        createPipelineLayout();
        createPipeline(renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem() {
        vkDestroyPipelineLayout(vulkrDevice.device(), pipelineLayout, nullptr);
    }

    void SimpleRenderSystem::createPipelineLayout() {
        VkPushConstantRange pushConstants{};
        pushConstants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstants.offset = 0;
        pushConstants.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstants;

        if (vkCreatePipelineLayout(vulkrDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void SimpleRenderSystem::createPipeline(VkRenderPass render_pass) {
        PipelineConfigInfo pipelineConfig{};
        VulkrPipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = render_pass;
        pipelineConfig.pipelineLayout = pipelineLayout;

        vulkrPipeline = std::make_unique<VulkrPipeline>(
            vulkrDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig
        );
    }

    void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject> &gameObjects,
                                               const Camera &camera) {
        vulkrPipeline->bind(commandBuffer);

        const auto projectionView = camera.getProjectionMatrix() * camera.getView();

        for (auto &obj: gameObjects) {
            SimplePushConstantData push{};
            auto modelMatrix = obj.transform.mat4();
            push.transform = projectionView * modelMatrix;
            push.normalMatrix = obj.transform.normalMatrix();
            push.enableLighting = obj.enableLighting ? 1 : 0;

            vkCmdPushConstants(
                commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
            );

            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }
    }
}
