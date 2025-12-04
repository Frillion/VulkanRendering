#ifndef WINDOW_MANAGER_HEADER
#define WINDOW_MANAGER_HEADER

#include <GLFW/glfw3.h>
#include <cstdint>
#include <stdexcept>

namespace FRI{

class WindowManager{
public:
    WindowManager(int width, int height, const std::string& name);

    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;

    bool window_open();
    void refresh();
    ~WindowManager();
private:
    GLFWwindow* main_window;
    uint32_t height,width;
    float fheight,fwidth;
};

}

#endif // !WINDOW_MANAGER_HEADER
