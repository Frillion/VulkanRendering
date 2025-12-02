cmake_minimum_required(VERSION 3.13)

project("Rendering Application")

add_executable(app main.cpp)

target_include_directories(app
    PRIVATE ${CMAKE_SOURCE_DIR}../Renderer/include/
)

target_link_directories(app PRIVATE ${CMAKE_SOURCE_DIR}../build/)
target_link_libraries(app Renderer)
