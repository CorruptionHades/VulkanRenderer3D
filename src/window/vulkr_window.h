//
// Created by CorruptionHades on 19/07/2025.
//

#ifndef VULKR_WINDOW_H
#define VULKR_WINDOW_H

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>

namespace vulkr {
  class VulkrWindow {
  public:
    VulkrWindow(int w, int h, std::string name);

    ~VulkrWindow();

    VulkrWindow(const VulkrWindow &) = delete;

    VulkrWindow &operator=(const VulkrWindow &) = delete;

    bool shouldClose() const { return glfwWindowShouldClose(window); }
    VkExtent2D getExtent() const { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
    bool wasFrameBufferResized() const { return frameBufferResized; }
    void resetFrameBufferResized() { frameBufferResized = false; }

    void resetMouseOffsets();

    glm::vec2 getMouseOffsets() const;

    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) const;

    GLFWwindow *getWindow() const { return window; }

  private:
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    static void mouseCallback(GLFWwindow *window, double xpos, double ypos);

    void initWindow();

    GLFWwindow *window;
    std::string windowName;
    int width;
    int height;
    bool frameBufferResized = false;

    double xOffset = 0.0;
    double yOffset = 0.0;
    double lastX = 0.0;
    double lastY = 0.0;
    bool firstMouse = true;
  };
}

#endif //VULKR_WINDOW_H
