cmake_minimum_required(VERSION 3.13)

add_library(${PROJECT_NAME} STATIC "window.cpp" "graphics_pipeline.cpp")

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_SOURCE_DIR}/Renderer/include
)

target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

