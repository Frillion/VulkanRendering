#include "graphics_pipeline.hpp"
#include "window.h"
#include <cstdio>

int main (int argc, char *argv[]) {
    FRI::WindowManager window_manager(800, 600, "Rendering Engine");    
    FRI::Pipeline("./simple_shader.vert.spv", "./simple_shader.frag.spv");
    while (window_manager.window_open()) {
        window_manager.refresh();
    }
    return 0;
}
