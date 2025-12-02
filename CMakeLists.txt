cmake_minimum_required(VERSION 3.13)

project("Renderer")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

add_subdirectory(${PROJECT_NAME})
add_subdirectory("Application")
