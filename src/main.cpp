//
// Created by corruptionhades on 19/07/2025.
//

#define GLFW_INCLUDE_VULKAN
#include "application.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

int main() {

    vulkr::Application app{};

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
