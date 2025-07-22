//
// Created by corru on 20/07/2025.
//

#include "MeshLoader.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <functional>

#include <glm/gtc/type_ptr.hpp>

#include "gltf_tiny/tiny_gltf.h"
#include "../utils/utils.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../mesh/tiny_obj_loader/tiny_obj_loader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace std {
    template<>
    struct hash<vulkr::VulkrModel::Vertex> {
        size_t operator()(const vulkr::VulkrModel::Vertex &vertex) const noexcept {
            size_t seed = 0;
            vulkr::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
}

namespace vulkr {
    VulkrModel::Builder loadModel(const std::string &path) {
        VulkrModel::Builder builder{};
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
            std::cerr << err << std::endl;
            throw std::runtime_error(warn + err);
        }

        builder.vertices.clear();
        builder.indices.clear();

        std::unordered_map<VulkrModel::Vertex, uint32_t> uniqueVertices{};

        for (const auto &shape: shapes) {
            for (const auto &index: shape.mesh.indices) {
                VulkrModel::Vertex vertex{};
                if (index.vertex_index >= 0) {
                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2],
                    };

                    vertex.color = {
                        attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2],
                    };
                }

                if (index.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2],
                    };
                }

                if (index.texcoord_index >= 0) {
                    vertex.uv = {
                        attrib.texcoords[3 * index.texcoord_index + 0],
                        attrib.texcoords[3 * index.texcoord_index + 1],
                    };
                }

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(builder.vertices.size());
                    builder.vertices.push_back(vertex);
                }
                builder.indices.push_back(uniqueVertices[vertex]);
            }
        }

        std::cout << "VulkrModel::Builder::loadModel: Loaded " << builder.vertices.size() << " unique vertices and "
                << builder.indices.size() << " indices from model file: " << path << std::endl;

        return builder;
    }

    std::unique_ptr<VulkrModel> MeshLoader::loadObjModel(VulkrDevice &device, const std::string &path) {
        VulkrModel::Builder builder = loadModel(path);

        std::cout << "VulkrModel::createModelFromFile: Loading model from file: " << path << std::endl;

        std::cout << std::endl << "VulkrModel::createModelFromFile: Loaded model with "
                << builder.vertices.size() << " vertices and "
                << builder.indices.size() << " indices." << std::endl;

        return std::make_unique<VulkrModel>(device, builder);
    }

    std::unique_ptr<VulkrModel> MeshLoader::loadGltfModel(VulkrDevice &device, const std::string &path) {
        VulkrModel::Builder builder{};
        std::unordered_map<VulkrModel::Vertex, uint32_t> uniqueVertices{};

        tinygltf::Model model;
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        stbi_set_flip_vertically_on_load(false); // glTF UVs are top-left origin

        bool res;
        if (path.substr(path.find_last_of('.') + 1) == "glb") {
            res = loader.LoadBinaryFromFile(&model, &err, &warn, path);
        } else {
            res = loader.LoadASCIIFromFile(&model, &err, &warn, path);
        }

        if (!res) {
            throw std::runtime_error("Failed to load glTF model: " + path + "\n" + err + warn);
        }

        if (!warn.empty()) {
            std::cout << "glTF Loader Warning: " << warn << std::endl;
        }
        if (!err.empty()) {
            std::cerr << "glTF Loader Error: " << err << std::endl;
        }

        // print all bones and their parents
        std::cout << "VulkrModel::Builder::loadGltfModel: Loading model from file: " << path << std::endl;
        for (const auto &skin: model.skins) {
            std::cout << "Skin: " << skin.name << std::endl;
            for (size_t i = 0; i < skin.joints.size(); ++i) {
                int jointIndex = skin.joints[i];
                const tinygltf::Node &jointNode = model.nodes[jointIndex];
                std::cout << "  Joint " << i << ": " << jointNode.name << " (index: " << jointIndex << ")" << std::endl;
            }
        }

        const tinygltf::Scene &scene = model.scenes[model.defaultScene > -1 ? model.defaultScene : 0];

        // Recursive function to process nodes
        std::function<void(int)> processNode =
                [&](int nodeIndex) {
            const tinygltf::Node &node = model.nodes[nodeIndex];
            if (node.mesh > -1) {
                const tinygltf::Mesh &mesh = model.meshes[node.mesh];
                for (const auto &primitive: mesh.primitives) {
                    if (primitive.mode != TINYGLTF_MODE_TRIANGLES) {
                        continue;
                    }

                    const float *positions = nullptr;
                    const float *normals = nullptr;
                    const float *texcoords = nullptr;
                    const float *colors = nullptr;
                    const uint16_t *joints = nullptr;
                    const float *weights = nullptr;
                    size_t vertexCount = 0;

                    if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                        const auto &accessor = model.accessors[primitive.attributes.at("POSITION")];
                        const auto &bufferView = model.bufferViews[accessor.bufferView];
                        positions = reinterpret_cast<const float *>(&(model.buffers[bufferView.buffer].data[
                            bufferView.byteOffset + accessor.byteOffset]));
                        vertexCount = accessor.count;
                    } else {
                        continue;
                    }

                    if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                        const auto &accessor = model.accessors[primitive.attributes.at("NORMAL")];
                        const auto &bufferView = model.bufferViews[accessor.bufferView];
                        normals = reinterpret_cast<const float *>(&(model.buffers[bufferView.buffer].data[
                            bufferView.byteOffset + accessor.byteOffset]));
                    }

                    if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                        const auto &accessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
                        const auto &bufferView = model.bufferViews[accessor.bufferView];
                        texcoords = reinterpret_cast<const float *>(&(model.buffers[bufferView.buffer].data[
                            bufferView.byteOffset + accessor.byteOffset]));
                    }

                    if (primitive.attributes.find("COLOR_0") != primitive.attributes.end()) {
                        const auto &colorAccessor = model.accessors[primitive.attributes.at("COLOR_0")];
                        const auto &colorBufferView = model.bufferViews[colorAccessor.bufferView];
                        if (colorAccessor.type == TINYGLTF_TYPE_VEC3) {
                            colors = reinterpret_cast<const float *>(&(model.buffers[colorBufferView.buffer].data[
                                colorBufferView.byteOffset + colorAccessor.byteOffset]));
                        }
                    }

                    if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end()) {
                        const auto &accessor = model.accessors[primitive.attributes.at("JOINTS_0")];
                        const auto &bufferView = model.bufferViews[accessor.bufferView];
                        joints = reinterpret_cast<const uint16_t *>(&(model.buffers[bufferView.buffer].data[
                            bufferView.byteOffset + accessor.byteOffset]));
                    }

                    if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end()) {
                        const auto &accessor = model.accessors[primitive.attributes.at("WEIGHTS_0")];
                        const auto &bufferView = model.bufferViews[accessor.bufferView];
                        weights = reinterpret_cast<const float *>(&(model.buffers[bufferView.buffer].data[
                            bufferView.byteOffset + accessor.byteOffset]));
                    }

                    if (primitive.indices > -1) {
                        const auto &indexAccessor = model.accessors[primitive.indices];
                        const auto &indexBufferView = model.bufferViews[indexAccessor.bufferView];
                        const auto &indexBuffer = model.buffers[indexBufferView.buffer];
                        const void *indexData = &(indexBuffer.data[
                            indexBufferView.byteOffset + indexAccessor.byteOffset]);

                        for (size_t i = 0; i < indexAccessor.count; ++i) {
                            uint32_t index;
                            switch (indexAccessor.componentType) {
                                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                                    index = static_cast<const uint32_t *>(indexData)[i];
                                    break;
                                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                                    index = static_cast<const uint16_t *>(indexData)[i];
                                    break;
                                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                                    index = static_cast<const uint8_t *>(indexData)[i];
                                    break;
                                default:
                                    continue;
                            }

                            VulkrModel::Vertex vertex{};
                            vertex.position = glm::make_vec3(&positions[index * 3]);

                            if (normals) {
                                vertex.normal = glm::make_vec3(&normals[index * 3]);
                            }

                            if (texcoords) {
                                vertex.uv = glm::make_vec2(&texcoords[index * 2]);
                            }

                            if (colors) {
                                vertex.color = glm::make_vec3(&colors[index * 3]);
                            } else {
                                vertex.color = {1.0f, 1.0f, 1.0f};
                            }

                            if (joints && weights) {
                                vertex.jointIndices = glm::vec4(joints[index * 4], joints[index * 4 + 1],
                                                                joints[index * 4 + 2], joints[index * 4 + 3]);
                                vertex.jointWeights = glm::make_vec4(&weights[index * 4]);
                            }

                            if (uniqueVertices.count(vertex) == 0) {
                                uniqueVertices[vertex] = static_cast<uint32_t>(builder.vertices.size());
                                builder.vertices.push_back(vertex);
                            }
                            builder.indices.push_back(uniqueVertices[vertex]);
                        }
                    } else {
                        for (size_t i = 0; i < vertexCount; i++) {
                            VulkrModel::Vertex vertex{};
                            vertex.position = glm::make_vec3(&positions[i * 3]);

                            if (normals) {
                                vertex.normal = glm::make_vec3(&normals[i * 3]);
                            }

                            if (texcoords) {
                                vertex.uv = glm::make_vec2(&texcoords[i * 2]);
                            }

                            if (colors) {
                                vertex.color = glm::make_vec3(&colors[i * 3]);
                            } else {
                                vertex.color = {1.0f, 1.0f, 1.0f};
                            }

                            if (joints && weights) {
                                vertex.jointIndices = glm::vec4(joints[i * 4], joints[i * 4 + 1], joints[i * 4 + 2],
                                                                joints[i * 4 + 3]);
                                vertex.jointWeights = glm::make_vec4(&weights[i * 4]);
                            }

                            builder.indices.push_back(static_cast<uint32_t>(builder.vertices.size()));
                            builder.vertices.push_back(vertex);
                        }
                    }
                }
            }
            for (int childIndex: node.children) {
                processNode(childIndex);
            }
        };

        for (int nodeIndex: scene.nodes) {
            processNode(nodeIndex);
        }

        // Load bone data
        if (!model.skins.empty()) {
            const tinygltf::Skin &skin = model.skins[0];
            builder.bones.resize(skin.joints.size());
            std::vector<glm::mat4> inverseBindMatrices(skin.joints.size());

            // Load inverse bind matrices
            if (skin.inverseBindMatrices > -1) {
                const auto &accessor = model.accessors[skin.inverseBindMatrices];
                const auto &bufferView = model.bufferViews[accessor.bufferView];
                const auto &buffer = model.buffers[bufferView.buffer];
                const auto *data = reinterpret_cast<const float *>(&buffer.data[
                    bufferView.byteOffset + accessor.byteOffset]);
                for (size_t i = 0; i < accessor.count; ++i) {
                    inverseBindMatrices[i] = glm::make_mat4(data + i * 16);
                }
            }

            // Build bone hierarchy
            for (size_t i = 0; i < skin.joints.size(); ++i) {
                int jointIndex = skin.joints[i];
                const tinygltf::Node &jointNode = model.nodes[jointIndex];
                builder.bones[i].transform = glm::mat4(1.0f);

                if (jointNode.matrix.size() == 16) {
                    builder.bones[i].transform = glm::make_mat4(jointNode.matrix.data());
                } else {
                    if (jointNode.translation.size() == 3) {
                        builder.bones[i].transform = glm::translate(builder.bones[i].transform,
                                                                    glm::vec3(jointNode.translation[0],
                                                                              jointNode.translation[1],
                                                                              jointNode.translation[2]));
                    }
                    if (jointNode.rotation.size() == 4) {
                        glm::quat q(static_cast<float>(jointNode.rotation[3]),
                                    static_cast<float>(jointNode.rotation[0]),
                                    static_cast<float>(jointNode.rotation[1]),
                                    static_cast<float>(jointNode.rotation[2]));
                        builder.bones[i].transform *= glm::mat4_cast(q);
                    }
                    if (jointNode.scale.size() == 3) {
                        builder.bones[i].transform = glm::scale(builder.bones[i].transform,
                                                                glm::vec3(jointNode.scale[0], jointNode.scale[1],
                                                                          jointNode.scale[2]));
                    }
                }

                builder.bones[i].transform = builder.bones[i].transform * inverseBindMatrices[i];

                builder.bones[i].parent = -1; // Set parent later
            }

            // Find parent for each bone
            for (size_t i = 0; i < skin.joints.size(); i++) {
                int joint_node_index = skin.joints[i];
                const auto &joint_node = model.nodes[joint_node_index];
                for (int child_node_index: joint_node.children) {
                    auto it = std::find(skin.joints.begin(), skin.joints.end(), child_node_index);
                    if (it != skin.joints.end()) {
                        int child_joint_index = std::distance(skin.joints.begin(), it);
                        builder.bones[child_joint_index].parent = i;
                    }
                }
            }

            // Create bone indices for rendering
            for (size_t i = 0; i < builder.bones.size(); ++i) {
                if (builder.bones[i].parent != -1) {
                    builder.boneIndices.push_back(i);
                    builder.boneIndices.push_back(builder.bones[i].parent);
                }
            }
        }

        // Load animations
        for (const auto &anim: model.animations) {
            VulkrModel::Builder::Animation animation{};
            animation.name = anim.name;

            for (const auto &sampler: anim.samplers) {
                VulkrModel::Builder::AnimationSampler animationSampler{};
                animationSampler.interpolation = sampler.interpolation;

                // Read sampler input time values
                {
                    const auto &accessor = model.accessors[sampler.input];
                    const auto &bufferView = model.bufferViews[accessor.bufferView];
                    const auto &buffer = model.buffers[bufferView.buffer];
                    const auto *data = reinterpret_cast<const float *>(&buffer.data[
                        bufferView.byteOffset + accessor.byteOffset]);
                    animationSampler.inputs.assign(data, data + accessor.count);
                }

                // Read sampler output values
                {
                    const auto &accessor = model.accessors[sampler.output];
                    const auto &bufferView = model.bufferViews[accessor.bufferView];
                    const auto &buffer = model.buffers[bufferView.buffer];
                    const auto *data = reinterpret_cast<const float *>(&buffer.data[
                        bufferView.byteOffset + accessor.byteOffset]);

                    switch (accessor.type) {
                        case TINYGLTF_TYPE_VEC3:
                            for (size_t i = 0; i < accessor.count; ++i) {
                                animationSampler.outputs.emplace_back(glm::make_vec3(data + i * 3), 0.0f);
                            }
                            break;
                        case TINYGLTF_TYPE_VEC4:
                            for (size_t i = 0; i < accessor.count; ++i) {
                                animationSampler.outputs.emplace_back(glm::make_vec4(data + i * 4));
                            }
                            break;
                        default:
                            break;
                    }
                }
                animation.samplers.push_back(animationSampler);
            }

            for (const auto &channel: anim.channels) {
                VulkrModel::Builder::AnimationChannel animationChannel{};
                animationChannel.path = channel.target_path;
                animationChannel.samplerIndex = channel.sampler;
                animationChannel.nodeIndex = channel.target_node;
                animation.channels.push_back(animationChannel);
            }

            builder.animations.push_back(animation);
        }

        std::cout << "VulkrModel::Builder::loadGltfModel: Loaded " << builder.vertices.size() << " unique vertices and "
                << builder.indices.size() << " indices from model file: " << path << std::endl;

        return std::make_unique<VulkrModel>(device, builder);
    }
}

