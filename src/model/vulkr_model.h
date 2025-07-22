//
// Created by CorruptionHades on 20/07/2025.
//

#ifndef MODEL_H
#define MODEL_H
#include "../pipeline/vulkr_device.hpp"

#define GLM_FORCE_RADIANS // force GLM to use radians for angles
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // force GLM to use depth range [0, 1]
#include <memory>
#include <glm/glm.hpp>

namespace vulkr {
    class VulkrModel {
    public:
        /**
         * Whenever you change the vertex structure, you must also update the binding and attribute descriptions.
         */
        struct Vertex {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};
            glm::vec4 jointIndices{0.0f};
            glm::vec4 jointWeights{0.0f};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();

            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

            bool operator==(const Vertex &other) const {
                return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
            }
        };

        struct Bone {
            glm::mat4 transform;
            int32_t parent;
        };

        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
            std::vector<Bone> bones{};
            std::vector<uint32_t> boneIndices{};

            struct AnimationSampler {
                std::string interpolation;
                std::vector<float> inputs;
                std::vector<glm::vec4> outputs;
            };

            struct AnimationChannel {
                std::string path;
                int samplerIndex;
                int nodeIndex;
            };

            struct Animation {
                std::string name;
                std::vector<AnimationSampler> samplers;
                std::vector<AnimationChannel> channels;
            };

            std::vector<Animation> animations;
        };

        VulkrModel(VulkrDevice &device, const Builder &builder);

        ~VulkrModel();

        VulkrModel(const VulkrModel &) = delete;

        VulkrModel &operator=(const VulkrModel &) = delete;

        void bind(VkCommandBuffer commandBuffer);

        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertexBuffers(const std::vector<Vertex> &vertices);

        void createIndexBuffers(const std::vector<uint32_t> &indices);

        void createBoneBuffers(const std::vector<Bone> &bones, const std::vector<uint32_t> &boneIndices);

        VulkrDevice &device;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;

        bool hasIndexBuffer{false};
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t indexCount;

        bool hasBoneBuffer{false};
        VkBuffer boneVertexBuffer;
        VkDeviceMemory boneVertexBufferMemory;
        uint32_t boneVertexCount;

        VkBuffer boneIndexBuffer;
        VkDeviceMemory boneIndexBufferMemory;
        uint32_t boneIndexCount;
    };
}


#endif //MODEL_H
