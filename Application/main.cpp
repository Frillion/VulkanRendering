#include "window.h"
#include <cstdio>

int main (int argc, char *argv[]) {
    FRI::WindowManager window_manager(800, 600, "Rendering Engine");    
    while (window_manager.window_open()) {
        window_manager.refresh();
    }
    return 0;
}
