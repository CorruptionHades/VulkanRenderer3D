//
// Created by corruptionhades on 20/07/2025.
//

#include <GLFW/glfw3.h>

#include "../game/camera.h"
#include "../game/game_object.h"

namespace vulkr {
    class CameraController {
    public:
        struct KeyMappings {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;

            int moveUp = GLFW_KEY_SPACE;
            int moveDown = GLFW_KEY_LEFT_SHIFT;
        } keys;

        void moveInPlaneXZ(GLFWwindow *window, float dt, GameObject &gameObject);

        float moveSpeed{3};
        float lookSpeed{1.5f};
    };
}
