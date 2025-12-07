#ifndef DEVICE_HEADER
#define DEVICE_HEADER
#include "window.h"
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

namespace FRI{

class Device{
public:
    Device(WindowManager& window, const std::string& application_name = "");
    Device(const Device& other) = delete;
    Device& operator=(const Device& other) = delete;
    ~Device();
private:
    struct QueueFamilyIndecies{
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> presentation_family;

        bool is_complete(){
            return graphics_family.has_value() && presentation_family.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    VkInstance instance;

    VkSurfaceKHR surface;
    WindowManager& window;

    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkDevice logical_device;

    VkQueue graphics_queue;
    VkQueue presentation_queue;

    void create_instance(const std::string& application_name);
    bool check_validation_layers();
    bool suitable_device(VkPhysicalDevice device);
    bool check_extension_support(VkPhysicalDevice device);
    void pick_physical_device();
    void create_logical_device();
    void create_surface();

    QueueFamilyIndecies find_queue_families(VkPhysicalDevice device);
    SwapChainSupportDetails query_swapchain_support(VkPhysicalDevice device);

    const std::vector<const char*> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

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
