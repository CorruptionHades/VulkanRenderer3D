//
// Created by CorruptionHades on 20/07/2025.
//

#ifndef CAMERA_H
#define CAMERA_H

#define GLM_FORCE_RADIANS // force GLM to use radians for angles
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // force GLM to use depth range [0, 1]
#include <glm/glm.hpp>

namespace vulkr {
    class Camera {
    public:
        void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);

        void setPerspectiveProjection(float fovy, float aspect, float near, float far);

        void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = {0, -1, 0});

        void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = {0, -1, 0});

        void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

        const glm::mat4 &getProjectionMatrix() const {
            return projectionMatrix;
        }

        const glm::mat4 &getView() const {
            return viewMatrix;
        }

    private:
        glm::mat4 projectionMatrix{1.0f};
        glm::mat4 viewMatrix{1.0f};
    };
}


#endif //CAMERA_H
