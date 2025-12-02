#ifndef WINDOW_MANAGER_HEADER
#define WINDOW_MANAGER_HEADER

#include <GLFW/glfw3.h>
#include <stdexcept>

namespace FRI{

class WindowManager{
public:
    WindowManager(int width, int height, const std::string& name);
    ~WindowManager();
private:
    GLFWwindow* main_window;
    int height,width;
    float fheight,fwidth;
};

}

#endif // !WINDOW_MANAGER_HEADER
