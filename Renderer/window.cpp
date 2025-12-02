#include "window.h"
#include <GLFW/glfw3.h>
#include <string>
namespace FRI{
WindowManager::WindowManager(int width, int height, const std::string& name){
    if(!glfwInit()){
        throw std::runtime_error("Failed To Initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    main_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
}

WindowManager::~WindowManager(){
    glfwDestroyWindow(main_window);
    glfwTerminate();
}

}
