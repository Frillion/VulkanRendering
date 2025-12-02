cmake_minimum_required(VERSION 3.8)

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(${PROJECT_NAME} "main.cpp")

target_link_directories(${PROJECT_NAME} 
    PUBLIC ${Vulkan_INCLUDE_DIRS}
    PUBLIC ${glm_INCLUDE_DIRS}
    PUBLIC ${glfw_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} 
    ${Vulkan_LIBRARIES}
    ${glfw_LIBRARIES} 
    ${glm_LIBRARIES}
)
