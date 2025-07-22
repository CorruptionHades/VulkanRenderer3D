//
// Created by CorruptionHades on 20/07/2025.
//

#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include "../model/vulkr_model.h"

namespace vulkr {
    struct TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{1, 1, 1};
        /**
         * Rotation in degrees around the x, y, and z axes.
         */
        glm::vec3 rotation{};

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 mat4();

        glm::mat3 normalMatrix();
    };

    class GameObject {
    public:
        using id_t = unsigned int;

        static GameObject createGameObject() {
            static id_t currentId = 0;
            return GameObject(currentId++);
        }

        GameObject(GameObject &) = delete;

        GameObject &operator=(const GameObject &) = delete;

        GameObject(GameObject &&) = default;

        GameObject &operator=(GameObject &&) = default;

        id_t getId() const { return id; }

        std::shared_ptr<VulkrModel> model{};
        glm::vec3 color{};
        TransformComponent transform{};

    private:
        explicit GameObject(id_t objId) : id(objId) {
        }

        id_t id;
    };
}


#endif //GAME_OBJECT_H
