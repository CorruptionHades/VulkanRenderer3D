//
// Created by corru on 20/07/2025.
//

#include "bone_render_system.h"

namespace vulkr {

    struct BonePushConstantData {
        glm::mat4 projectionView{1.f};
    };

    BoneRenderSystem::BoneRenderSystem(VulkrDevice &device, VkRenderPass renderPass)
        : vulkrDevice(device) {
        createPipelineLayout();
        createPipeline(renderPass);
    }

    BoneRenderSystem::~BoneRenderSystem() {
        vkDestroyPipelineLayout(vulkrDevice.device(), pipelineLayout, nullptr);
    }

    void BoneRenderSystem::createPipelineLayout() {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(BonePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(vulkrDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create bone pipeline layout!");
        }
    }

    void BoneRenderSystem::createPipeline(VkRenderPass renderPass) {
        PipelineConfigInfo pipelineConfig{};
        VulkrPipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipelineConfig.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

        vulkrPipeline = std::make_unique<VulkrPipeline>(
            vulkrDevice,
            "shaders/bone_shader.vert.spv",
            "shaders/bone_shader.frag.spv",
            pipelineConfig);
    }

    void BoneRenderSystem::renderBones(VkCommandBuffer commandBuffer, VulkrModel &model, const Camera &camera) {
        vulkrPipeline->bind(commandBuffer);

        BonePushConstantData push{};
        push.projectionView = camera.getProjectionMatrix() * camera.getView();

        vkCmdPushConstants(
            commandBuffer,
            pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT,
            0,
            sizeof(BonePushConstantData),
            &push);

        model.bind(commandBuffer);
        model.draw(commandBuffer);
    }
}
