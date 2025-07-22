//
// Created by CorruptionHades on 22/07/2025.
//

#include "hud_render_system.h"
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include <array>
#include <iostream>
#include <string>

namespace vulkr {
    // Push constant structure for the HUD
    struct HudPushConstantData {
        glm::mat4 transform{1.0f};
    };

    // Vertex definitions for numbers 0-9
    // Each number is defined within a 1x1 box
    namespace {
        // --- 0 ---
        const std::vector<VulkrModel::Vertex> vertices_0 = {
            {{-0.5f, 0.0f, 0}}, {{-0.25f, 0.5f, 0}}, {{0.25f, 0.5f, 0}}, {{0.5f, 0.0f, 0}},
            {{0.25f, -0.5f, 0}}, {{-0.25f, -0.5f, 0}}
        };
        const std::vector<uint32_t> indices_0 = {
            0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 0
        };

        // --- 1 ---
        const std::vector<VulkrModel::Vertex> vertices_1 = {
            {{0.0f, -0.5f, 0}}, {{0.0f, 0.5f, 0}}
        };
        const std::vector<uint32_t> indices_1 = {
            0, 1
        };

        // --- 2 ---
        const std::vector<VulkrModel::Vertex> vertices_2 = {
            {{-0.5f, -0.5f, 0}}, {{0.5f, -0.5f, 0}}, {{0.5f, 0.0f, 0}}, {{-0.5f, 0.0f, 0}},
            {{-0.5f, 0.5f, 0}}, {{0.5f, 0.5f, 0}}
        };
        const std::vector<uint32_t> indices_2 = {
            0, 1, 1, 2, 2, 3, 3, 4, 4, 5
        };

        // --- 3 ---
        const std::vector<VulkrModel::Vertex> vertices_3 = {
            {{-0.5f, -0.5f, 0}}, {{0.5f, -0.5f, 0}}, {{0.0f, 0.0f, 0}}, {{0.5f, 0.0f, 0}},
            {{-0.5f, 0.5f, 0}}, {{0.5f, 0.5f, 0}}
        };
        const std::vector<uint32_t> indices_3 = {
            0, 1, 1, 2, 2, 3, 3, 5, 5, 4
        };

        // --- 4 ---
        // Corrected --- 4 ---
        const std::vector<VulkrModel::Vertex> vertices_4 = {
            {{-0.5f, -0.5f, 0}}, // top-left
            {{-0.5f, 0.0f, 0}}, // mid-left
            {{0.5f, -0.5f, 0}}, // top-right
            {{0.5f, 0.0f, 0}}, // mid-right
            {{0.5f, 0.5f, 0}} // bottom-right
        };
        const std::vector<uint32_t> indices_4 = {
            0, 1, // Top-left vertical line
            1, 3, // Middle horizontal line
            2, 3, // Top-right vertical line
            3, 4 // Bottom-right vertical line
        };

        // --- 5 ---
        const std::vector<VulkrModel::Vertex> vertices_5 = {
            {{0.5f, -0.5f, 0}}, {{-0.5f, -0.5f, 0}}, {{-0.5f, 0.0f, 0}}, {{0.5f, 0.0f, 0}},
            {{0.5f, 0.5f, 0}}, {{-0.5f, 0.5f, 0}}
        };
        const std::vector<uint32_t> indices_5 = {
            0, 1, 1, 2, 2, 3, 3, 4, 4, 5
        };

        // --- 6 ---
        const std::vector<VulkrModel::Vertex> vertices_6 = {
            {{0.5f, -0.5f, 0}}, {{-0.5f, -0.5f, 0}}, {{-0.5f, 0.5f, 0}}, {{0.5f, 0.5f, 0}},
            {{0.5f, 0.0f, 0}}, {{-0.5f, 0.0f, 0}}
        };
        const std::vector<uint32_t> indices_6 = {
            0, 1, 1, 2, 2, 3, 3, 4, 4, 5
        };

        // --- 7 ---
        const std::vector<VulkrModel::Vertex> vertices_7 = {
            {{-0.5f, -0.5f, 0}}, {{0.5f, -0.5f, 0}}, {{0.0f, 0.5f, 0}}
        };
        const std::vector<uint32_t> indices_7 = {
            0, 1, 1, 2
        };

        // --- 8 ---
        const std::vector<VulkrModel::Vertex> vertices_8 = {
            {{-0.5f, -0.5f, 0}}, {{0.5f, -0.5f, 0}}, {{0.5f, 0.5f, 0}}, {{-0.5f, 0.5f, 0}},
            {{-0.5f, 0.0f, 0}}, {{0.5f, 0.0f, 0}}
        };
        const std::vector<uint32_t> indices_8 = {
            0, 1, 1, 2, 2, 3, 3, 0, 4, 5
        };

        // --- 9 ---
        const std::vector<VulkrModel::Vertex> vertices_9 = {
            {{-0.5f, 0.5f, 0}}, {{0.5f, 0.5f, 0}}, {{0.5f, -0.5f, 0}}, {{-0.5f, -0.5f, 0}},
            {{-0.5f, 0.0f, 0}}, {{0.5f, 0.0f, 0}}
        };
        const std::vector<uint32_t> indices_9 = {
            0, 1, 1, 2, 2, 3, 3, 4, 4, 5
        };
    } // anonymous namespace

    HudRenderSystem::HudRenderSystem(VulkrDevice &device, VkRenderPass renderPass)
        : vulkrDevice{device} {
        createPipelineLayout();
        createPipeline(renderPass);

        // Create crosshair model
        // Create crosshair model as a cross
        std::vector<VulkrModel::Vertex> crosshairVertices = {
            {{-0.02f, 0.0f, 0.0f}}, {{0.02f, 0.0f, 0.0f}}, // horizontal line
            {{0.0f, -0.02f, 0.0f}}, {{0.0f, 0.02f, 0.0f}} // vertical line
        };
        std::vector<uint32_t> crosshairIndices = {0, 1, 2, 3};
        VulkrModel::Builder crosshairBuilder{};
        crosshairBuilder.vertices = std::move(crosshairVertices);
        crosshairBuilder.indices = std::move(crosshairIndices);
        crosshairModel = std::make_unique<VulkrModel>(vulkrDevice, crosshairBuilder);

        // Create digit models
        auto create_digit_model = [&](const auto &vertices, const auto &indices) {
            VulkrModel::Builder builder{};
            builder.vertices = vertices;
            builder.indices = indices;
            return std::make_unique<VulkrModel>(vulkrDevice, builder);
        };

        // 0 1 2 3 4 5 6 7 8 9
        // 0 1 2 3 4 5 8 7 6 9

        digitModels[0] = create_digit_model(vertices_0, indices_0);
        digitModels[1] = create_digit_model(vertices_1, indices_1);
        digitModels[2] = create_digit_model(vertices_2, indices_2);
        digitModels[3] = create_digit_model(vertices_3, indices_3);
        digitModels[4] = create_digit_model(vertices_4, indices_4);
        digitModels[5] = create_digit_model(vertices_5, indices_5);
        digitModels[6] = create_digit_model(vertices_6, indices_6);
        digitModels[7] = create_digit_model(vertices_7, indices_7);
        digitModels[8] = create_digit_model(vertices_8, indices_8);
        digitModels[9] = create_digit_model(vertices_9, indices_9);
    }

    HudRenderSystem::~HudRenderSystem() {
        vkDestroyPipelineLayout(vulkrDevice.device(), pipelineLayout, nullptr);
    }

    void HudRenderSystem::createPipelineLayout() {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(HudPushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(vulkrDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void HudRenderSystem::createPipeline(VkRenderPass renderPass) {
        PipelineConfigInfo pipelineConfig{};
        VulkrPipeline::defaultPipelineConfigInfo(pipelineConfig);

        // Configure the pipeline for HUD rendering
        pipelineConfig.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipelineConfig.depthStencilInfo.depthTestEnable = VK_FALSE;
        pipelineConfig.depthStencilInfo.depthWriteEnable = VK_FALSE;
        pipelineConfig.colorBlendAttachment.blendEnable = VK_TRUE;

        hudPipeline = std::make_unique<VulkrPipeline>(
            vulkrDevice,
            "shaders/hud.vert.spv",
            "shaders/hud.frag.spv",
            pipelineConfig
        );
    }

    void HudRenderSystem::render(VkCommandBuffer commandBuffer, float aspect) {
        hudPipeline->bind(commandBuffer);

        // Render crosshair
        HudPushConstantData push{};
        glm::mat4 crosshairMatrix = glm::scale(glm::mat4(1.0f), {1.0f / aspect, 1.0f, 1.0f});
        push.transform = crosshairMatrix;
        vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(HudPushConstantData),
                           &push);
        crosshairModel->bind(commandBuffer);
        crosshairModel->draw(commandBuffer);
    }

    void HudRenderSystem::renderNumber(VkCommandBuffer commandBuffer, int number, float x, float y, float scale,
                                       float aspect) {
        hudPipeline->bind(commandBuffer);

        std::string numStr = std::to_string(number);
        float currentX = x;

        for (char &digitChar: numStr) {
            int digit = digitChar - '0';
            if (digit < 0 || digit > 9) continue; // Skip non-digit characters

            HudPushConstantData push{};
            auto modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, {currentX, y, 0.0f});
            modelMatrix = glm::scale(modelMatrix, {scale / aspect, scale, 1.0f});

            push.transform = modelMatrix;

            vkCmdPushConstants(
                commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT,
                0,
                sizeof(HudPushConstantData),
                &push);

            digitModels[digit]->bind(commandBuffer);
            digitModels[digit]->draw(commandBuffer);

            // Advance cursor position for the next digit
            currentX += (scale + (scale * 0.3f));
        }
    }
}
