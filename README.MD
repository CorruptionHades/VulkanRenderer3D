# Vulkan Renderer 3D

Simple 3D renderer using Vulkan API based on **Brendan Galea's**
[tutorial series](https://www.youtube.com/playlist?list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR).

## Features

- Basic Camera Controls
- Basic Lighting
- Display .obj and .gltf models
- Basic HUD display with crosshair and fps display

## Requirements

- [Vulkan SDK 1.4.321.1](https://vulkan.lunarg.com/sdk/home)
- [GLFW 3.4](https://github.com/glfw/glfw/releases/tag/3.4)
- [GLM 0.9.9.8](https://github.com/g-truc/glm/releases/tag/0.9.9.8)

## Building

1. Install the Vulkan SDK, GLFW, and GLM.
2. In the `CMakeLists.txt`, set the paths to the Vulkan SDK, GLFW, and GLM.
3. Run `cmake .` in the project directory.
4. Run ``cmake --build .`` to build the project.
5. Copy the `models/` directory to the build directory.
6. Run the executable.

## BDI

- Tested on Windows 11 with [Clion](https://www.jetbrains.com/clion/download/?section=windows)
- Models provided in the `models/` directory are from the tutorial series.
