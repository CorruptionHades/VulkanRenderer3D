//
// Created by CorruptionHades on 19/07/2025.
//

#ifndef VULKR_PIPELINE_H
#define VULKR_PIPELINE_H
#include <string>
#include <vector>
#include "vulkr_device.hpp"

namespace vulkr {
    struct PipelineConfigInfo {
        PipelineConfigInfo() = default;

        PipelineConfigInfo(const PipelineConfigInfo &) = delete;

        PipelineConfigInfo &operator=(const PipelineConfigInfo &) = delete;

        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;

        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;

        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class VulkrPipeline {
    public:
        VulkrPipeline(VulkrDevice &device, const std::string &vertFilepath, const std::string &fragFilepath,
                      const PipelineConfigInfo &configInfo);

        ~VulkrPipeline();

        VulkrPipeline(const VulkrPipeline &) = delete;

        VulkrPipeline &operator=(const VulkrPipeline &) = delete;

        void bind(VkCommandBuffer commandBuffer);

        static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);

    private:
        static std::vector<char> readFile(const std::string &filepath);

        void createGraphicsPipeline(const std::string &vertFilepath, const std::string &fragFilepath,
                                    const PipelineConfigInfo &configInfo);

        void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

        VulkrDevice &vulkrDevice;
        VkPipeline graphicsPipeline;

        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };
}


#endif //VULKR_PIPELINE_H
