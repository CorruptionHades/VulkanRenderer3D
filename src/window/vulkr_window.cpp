//
// Created by CorruptionHades on 19/07/2025.
//

#include "vulkr_window.h"
#include <stdexcept>

namespace vulkr {
    VulkrWindow::VulkrWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
        initWindow();
    }

    VulkrWindow::~VulkrWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void VulkrWindow::resetMouseOffsets() {
        xOffset = 0.0;
        yOffset = 0.0;
    }

    glm::vec2 VulkrWindow::getMouseOffsets() const {
        return glm::vec2(xOffset, yOffset);
    }

    void VulkrWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) const {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface!");
        }
    }

    void VulkrWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
        auto vulkrWindow = static_cast<VulkrWindow *>(glfwGetWindowUserPointer(window));
        vulkrWindow->frameBufferResized = true;
        vulkrWindow->width = width;
        vulkrWindow->height = height;
    }

    void VulkrWindow::mouseCallback(GLFWwindow *window, double xpos, double ypos) {
        auto *vulkrWindow = static_cast<VulkrWindow *>(glfwGetWindowUserPointer(window));
        if (vulkrWindow->firstMouse) {
            vulkrWindow->lastX = xpos;
            vulkrWindow->lastY = ypos;
            vulkrWindow->firstMouse = false;
        }
        vulkrWindow->xOffset = xpos - vulkrWindow->lastX;
        vulkrWindow->yOffset = vulkrWindow->lastY - ypos; // Reversed since y-coordinates
        vulkrWindow->lastX = xpos;
        vulkrWindow->lastY = ypos;
    }

    void VulkrWindow::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouseCallback);
    }
}
