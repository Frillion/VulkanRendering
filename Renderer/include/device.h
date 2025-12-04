#ifndef DEVICE_HEADER
#define DEVICE_HEADER
#include "window.h"
#include <string>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

namespace FRI{

class Device{
public:
    Device(const std::string& application_name);
    ~Device();
private:
    VkInstance instance;
    void create_instance(const std::string& application_name);
    bool check_validation_layers();

    const std::vector<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    const bool enable_validation_layers = false;
#else
    const bool enable_validation_layers = true;
#endif

};

}

#endif // !DEVICE_HEADER
