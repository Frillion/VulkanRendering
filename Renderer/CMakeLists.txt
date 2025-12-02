cmake_minimum_required(VERSION 3.13)

add_library(${PROJECT_NAME} "window.cpp")

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_SOURCE_DIR}/Renderer/include
)

target_link_directories(${PROJECT_NAME} 
    PUBLIC ${Vulkan_INCLUDE_DIRS} 
    PUBLIC ${glfw3_INCLUDE_DIRS} 
    PUBLIC ${glm_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} 
    ${Vulkan_LIBRARIES} 
    ${glfw3_LIBRARIES} 
    ${glm_LIBRARIES}
)

