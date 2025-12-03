#include "window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <string>
namespace FRI{

inline void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

WindowManager::WindowManager(int width, int height, const std::string& name): width(width), height(height){
    if(glfwInit() == -1){ throw std::runtime_error("Failed To Initialize GLFW"); }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    main_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

    if(!main_window){ throw std::runtime_error("Failed To Create Window"); }

    glfwSetWindowUserPointer(main_window, this);
    glfwSwapInterval(1);
}

bool WindowManager::window_open(){
    return !glfwWindowShouldClose(main_window);
}

void WindowManager::refresh(){
    glfwPollEvents();
}

WindowManager::~WindowManager(){
    glfwDestroyWindow(main_window);
    glfwTerminate();
}

}
