//
// Created by corru on 19/07/2025.
//

#ifndef FIRST_APP_H
#define FIRST_APP_H
#include "window/vulkr_window.h"
#include "pipeline/vulkr_swap_chain.hpp"

#include <memory>
#include <vector>

#include "game/game_object.h"
#include "render/vulkr_renderer.h"

namespace vulkr {
    class Application {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 800;

        Application();

        ~Application();

        Application(const Application &) = delete;

        Application &operator=(const Application &) = delete;

        void run();

        void update(float dt);

    private:
        void loadGameObjects();

        VulkrWindow vulkrWindow{WIDTH, HEIGHT, "First App"};
        VulkrDevice vulkrDevice{vulkrWindow};
        VulkrRenderer vulkrRenderer{vulkrWindow, vulkrDevice};

        std::vector<GameObject> gameObjects;
    };
}

#endif //FIRST_APP_H
