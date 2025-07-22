//
// Created by corruptionhades on 20/07/2025.
//

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "vulkr_model.h"
#include <iostream>

namespace vulkr {
    std::vector<VkVertexInputBindingDescription> VulkrModel::Vertex::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingsDescriptions(1);
        bindingsDescriptions[0].binding = 0;
        bindingsDescriptions[0].stride = sizeof(Vertex);
        bindingsDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingsDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> VulkrModel::Vertex::getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
        attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
        attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});
        attributeDescriptions.push_back({4, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, jointIndices)});
        attributeDescriptions.push_back({5, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, jointWeights)});

        return attributeDescriptions;
    }

    VulkrModel::VulkrModel(VulkrDevice &device, const Builder &builder) : device(device) {
        createVertexBuffers(builder.vertices);
        createIndexBuffers(builder.indices);
    }

    VulkrModel::~VulkrModel() {
        vkDestroyBuffer(device.device(), vertexBuffer, nullptr);
        vkFreeMemory(device.device(), vertexBufferMemory, nullptr);
        if (hasIndexBuffer) {
            vkDestroyBuffer(device.device(), indexBuffer, nullptr);
            vkFreeMemory(device.device(), indexBufferMemory, nullptr);
        }
    }

    void VulkrModel::bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        if (hasIndexBuffer) {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void VulkrModel::draw(VkCommandBuffer commandBuffer) {
        if (hasIndexBuffer) {
            vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
        } else vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }

    void VulkrModel::createVertexBuffers(const std::vector<Vertex> &vertices) {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 2 && "Vertex count must be at least 3 to form a triangle");

        VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        device.createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            stagingBuffer, stagingBufferMemory
        );

        void *data;
        vkMapMemory(device.device(), stagingBufferMemory, 0, vertexBufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(vertexBufferSize));
        vkUnmapMemory(device.device(), stagingBufferMemory);

        device.createBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                            vertexBuffer, vertexBufferMemory
        );

        device.copyBuffer(stagingBuffer, vertexBuffer, vertexBufferSize);

        // clean up staging buffer
        vkDestroyBuffer(device.device(), stagingBuffer, nullptr);
        vkFreeMemory(device.device(), stagingBufferMemory, nullptr);
    }

    void VulkrModel::createIndexBuffers(const std::vector<uint32_t> &indices) {
        indexCount = static_cast<uint32_t>(indices.size());
        hasIndexBuffer = indexCount > 0;

        if (!hasIndexBuffer) {
            return;
        }

        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        device.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            stagingBuffer, stagingBufferMemory
        );

        void *data;
        vkMapMemory(device.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(device.device(), stagingBufferMemory);

        device.createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                            indexBuffer, indexBufferMemory
        );

        device.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

        // clean up staging buffer
        vkDestroyBuffer(device.device(), stagingBuffer, nullptr);
        vkFreeMemory(device.device(), stagingBufferMemory, nullptr);
    }

    void VulkrModel::createBoneBuffers(const std::vector<Bone> &bones, const std::vector<uint32_t> &boneIndices) {
        boneVertexCount = static_cast<uint32_t>(bones.size());
        hasBoneBuffer = boneVertexCount > 0;

        if (!hasBoneBuffer) {
            return;
        }

        VkDeviceSize vertexBufferSize = sizeof(bones[0]) * boneVertexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        device.createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            stagingBuffer, stagingBufferMemory
        );

        void *data;
        vkMapMemory(device.device(), stagingBufferMemory, 0, vertexBufferSize, 0, &data);
        memcpy(data, bones.data(), static_cast<size_t>(vertexBufferSize));
        vkUnmapMemory(device.device(), stagingBufferMemory);

        device.createBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                            boneVertexBuffer, boneVertexBufferMemory
        );

        device.copyBuffer(stagingBuffer, boneVertexBuffer, vertexBufferSize);

        // clean up staging buffer
        vkDestroyBuffer(device.device(), stagingBuffer, nullptr);
        vkFreeMemory(device.device(), stagingBufferMemory, nullptr);

        boneIndexCount = static_cast<uint32_t>(boneIndices.size());
        if (boneIndexCount > 0) {
            VkDeviceSize indexBufferSize = sizeof(boneIndices[0]) * boneIndexCount;

            VkBuffer indexStagingBuffer;
            VkDeviceMemory indexStagingBufferMemory;
            device.createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                indexStagingBuffer, indexStagingBufferMemory
            );

            vkMapMemory(device.device(), indexStagingBufferMemory, 0, indexBufferSize, 0, &data);
            memcpy(data, boneIndices.data(), static_cast<size_t>(indexBufferSize));
            vkUnmapMemory(device.device(), indexStagingBufferMemory);

            device.createBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                boneIndexBuffer, boneIndexBufferMemory
            );

            device.copyBuffer(indexStagingBuffer, boneIndexBuffer, indexBufferSize);

            vkDestroyBuffer(device.device(), indexStagingBuffer, nullptr);
            vkFreeMemory(device.device(), indexStagingBufferMemory, nullptr);
        }
    }
}
