//
// Created by corru on 19/07/2025.
//

#include "application.h"

#include <array>
#include <functional>
#include <stdexcept>
#include <chrono>

#define GLM_FORCE_RADIANS // force GLM to use radians for angles
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // force GLM to use depth range [0, 1]
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "input/camera_controller.h"
#include "mesh/MeshLoader.h"
#include "render/simple_render_system.h"
#include "render/bone_render_system.h"
#include "render/hud/hud_render_system.h"

namespace vulkr {
    Application::Application() {
        loadGameObjects();
    }

    Application::~Application() {
    }

    void Application::run() {
        SimpleRenderSystem simpleRenderSystem{vulkrDevice, vulkrRenderer.getSwapChainRenderPass()};
        BoneRenderSystem boneRenderSystem{vulkrDevice, vulkrRenderer.getSwapChainRenderPass()};
        HudRenderSystem hudRenderSystem{vulkrDevice, vulkrRenderer.getSwapChainRenderPass()};
        Camera camera{};

        auto viewerObject = GameObject::createGameObject();
        CameraController cameraController{};

        using clock = std::chrono::high_resolution_clock;
        auto currentTime = clock::now();
        int frameCount = 0;
        float fpsTimer = 0.0f;
        int fps = 0;

        while (!vulkrWindow.shouldClose()) {
            glfwPollEvents();

            auto newTime = clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            update(frameTime);

            cameraController.moveInPlaneXZ(vulkrWindow.getWindow(), frameTime, viewerObject);
            vulkrWindow.resetMouseOffsets();
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = vulkrRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);

            if (auto commandBuffer = vulkrRenderer.beginFrame()) {
                vulkrRenderer.beginSwapChainRenderPass(commandBuffer);

                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);

                hudRenderSystem.renderNumber(commandBuffer, fps, -0.95f, 0.9f, 0.03f, aspect);
                hudRenderSystem.render(commandBuffer, aspect);

                vulkrRenderer.endSwapChainRenderPass(commandBuffer);
                vulkrRenderer.endFrame();
            } else {
                continue;
            }

            fpsTimer += frameTime;
            frameCount++;
            if (fpsTimer >= 1.0f) {
                std::cout << "FPS: " << frameCount << std::endl;
                fps = frameCount;
                frameCount = 0;
                fpsTimer = 0.0f;
            }
        }

        vkDeviceWaitIdle(vulkrDevice.device());
    }

    void Application::update(float dt) {
        gameObjects.at(0).transform.rotation.y += 1 * dt; // Rotate the first object around the Y-axis
    }

    void Application::loadGameObjects() {
        std::shared_ptr<VulkrModel> vulkrModel = MeshLoader::loadObjModel(vulkrDevice, "models/FinalBaseMesh.obj");

        auto obj = GameObject::createGameObject();
        obj.model = vulkrModel;
        obj.transform.translation = {0, 0, 2.5f};
        obj.transform.scale = {.2f, .2f, .2f};
        obj.transform.rotation = {0, 0, glm::radians(180.0f)};

        gameObjects.push_back(std::move(obj));

        std::shared_ptr<VulkrModel> model = MeshLoader::loadObjModel(vulkrDevice, "models/bikini/combined.obj");

        auto ob = GameObject::createGameObject();
        ob.model = model;
        ob.transform.translation = {0, 0, -2.5f};
        ob.transform.scale = {.2f, .2f, .2f};
        ob.transform.rotation = {glm::radians(90.0f), 0, 0};

        gameObjects.push_back(std::move(ob));
    }
}
