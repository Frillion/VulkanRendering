#ifndef DEVICE_HEADER
#define DEVICE_HEADER
#include "window.h"
#include <optional>
#include <string>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

namespace FRI{

class Device{
public:
    Device(const std::string& application_name);
    Device(const Device& other) = delete;
    Device& operator=(const Device& other) = delete;
    ~Device();
private:
    struct QueueFamilyIndecies{
        std::optional<uint32_t> graphics_family;
        bool is_complete(){
            return graphics_family.has_value();
        }
    };

    VkInstance instance;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkDevice logical_device;
    VkQueue graphics_queue;

    void create_instance(const std::string& application_name);
    bool check_validation_layers();
    bool suitable_device(VkPhysicalDevice device);
    QueueFamilyIndecies find_queue_families(VkPhysicalDevice device);
    void pick_physical_device();
    void create_logical_device();

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
