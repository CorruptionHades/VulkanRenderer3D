cmake_minimum_required(VERSION 3.20)
project(VulkanRenderer3D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
set(Vulkan_LIBRARY "H:/C++/_Libraries_/VulkanSDK/1.4.321.1/Lib/vulkan-1.lib")
set(Vulkan_INCLUDE_DIR "H:/C++/_Libraries_/VulkanSDK/1.4.321.1/Include")

set(GLFW_INCLUDE_DIR "H:/C++/_Libraries_/glfw-3.4.bin.WIN64/include")
set(GLFW_LIBRARIES "H:/C++/_Libraries_/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib")

set(GLM_DIR "H:/C++/_Libraries_/glm/")
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SRC
        src/*.cpp
        src/*.h
        src/*.hpp
)

add_executable(${PROJECT_NAME} ${SRC} ${GLM_DIR})

# Include directories
include_directories(
        ${GLFW_INCLUDE_DIR}
)

if (MINGW)
    target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++ --static)
endif ()

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Vulkan::Vulkan
        ${GLFW_LIBRARIES}
)

# compile shaders using glslc
find_program(GLSLC_EXECUTABLE glslc)
if (NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc not found. Please install the Vulkan SDK and ensure it is in your PATH envvar.")
endif ()

# Define shader files
file(GLOB_RECURSE SHADER_FILES
        shaders/*.vert shaders/*.frag shaders/*.comp shaders/*.geom shaders/*.tesc shaders/*.tese
)
find_package(Vulkan COMPONENTS glslc REQUIRED)

set(SPIRV_FILES "")
foreach (SHADER_FILE ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
    get_filename_component(SHADER_EXT ${SHADER_FILE} EXT)
    string(SUBSTRING ${SHADER_EXT} 1 -1 SHADER_EXT_NO_DOT)
    set(SPIRV_FILE "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.${SHADER_EXT_NO_DOT}.spv")

    add_custom_command(
            OUTPUT ${SPIRV_FILE}
            COMMAND ${GLSLC_EXECUTABLE} -o ${SPIRV_FILE} ${SHADER_FILE}
            DEPENDS ${SHADER_FILE}
            COMMENT "Compiling shader: ${SHADER_FILE}"
    )

    list(APPEND SPIRV_FILES ${SPIRV_FILE})
endforeach ()

add_custom_target(shaders ALL DEPENDS ${SPIRV_FILES})
add_dependencies(${PROJECT_NAME} shaders)
