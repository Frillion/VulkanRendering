#include "device.h"
#include "window.h"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
namespace FRI{

Device::Device(WindowManager& window,const std::string& application_name):window(window){
    create_instance(application_name);
    create_surface();
    pick_physical_device();
    create_logical_device();
}

void Device::create_instance(const std::string& application_name){
    // Check If Requested Validation Layers Are Available
    if(enable_validation_layers && !check_validation_layers()){
        throw std::runtime_error("Requested Validation Layers Are Not Available");
    }

    // Set Information About The Application 
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = application_name.c_str();
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_4;

    // Load Window API extensions for Vulkan since it is platform agnostic
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions;
    glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    // Set State For Creating The Vulkan Instance
    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = glfw_extension_count;
    create_info.ppEnabledExtensionNames = glfw_extensions;
    // Enable Validation Layers If Debug Is Enabled
    if(enable_validation_layers){
        create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();
    } else{
        create_info.enabledLayerCount = 0;
    }

    // Create The Vulkan Instance And Validate
    VkResult status = vkCreateInstance(&create_info, nullptr, &instance);
    if(status != VK_SUCCESS){
        throw std::runtime_error("Failed To Create Vulkan Instance");
    }

    // Check Available Extensions
    uint32_t vulkan_extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &vulkan_extension_count, nullptr);
    std::vector<VkExtensionProperties> extensions(vulkan_extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &vulkan_extension_count, extensions.data());

    std::cout << "Available Extensions:\n";

    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }
}

void Device::create_logical_device(){
    QueueFamilyIndecies indecies = find_queue_families(physical_device);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = {indecies.graphics_family.value(),indecies.presentation_family.value()};
    float queue_priority = 1.0f;// Queue priority from 0.0 - 1.0 for command buffer scheduling
    for(uint32_t queue_family : unique_queue_families){
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family; 
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
    }

    // Select GPU Features We Want To Use
    VkPhysicalDeviceFeatures device_features{};

    // Logical Device Creation Information
    VkDeviceCreateInfo device_create_info{};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pQueueCreateInfos = queue_create_infos.data();
    device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    device_create_info.pEnabledFeatures = &device_features;
    device_create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    device_create_info.ppEnabledExtensionNames = device_extensions.data();

    if (enable_validation_layers) {
        device_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        device_create_info.ppEnabledLayerNames = validation_layers.data();
    } else {
        device_create_info.enabledLayerCount = 0;
    }

    if(vkCreateDevice(physical_device, &device_create_info, nullptr, &logical_device) != VK_SUCCESS){
        throw std::runtime_error("Failed To Create Logical Device");
    }

    vkGetDeviceQueue(logical_device, indecies.graphics_family.value(), 0, &graphics_queue);
    vkGetDeviceQueue(logical_device, indecies.presentation_family.value(), 0, &presentation_queue);
}

bool Device::check_validation_layers(){
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validation_layers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

void Device::pick_physical_device(){
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

    if(device_count == 0){
        throw std::runtime_error("Failed To Find Vulkan Compatible Graphics Device");
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    for(const auto& device:devices){
        if(suitable_device(device)){
            physical_device = device;
            break;
        }
    }

    if(physical_device == VK_NULL_HANDLE){
        throw std::runtime_error("Failed To Find Vulkan Compatible Graphics Device");
    }
}

bool Device::suitable_device(VkPhysicalDevice device){
    QueueFamilyIndecies indecies = find_queue_families(device);

    VkPhysicalDeviceProperties2 device_properties;
    VkPhysicalDeviceFeatures2 device_features;
    vkGetPhysicalDeviceProperties2(device, &device_properties);
    vkGetPhysicalDeviceFeatures2(device, &device_features);

    bool extensions_supported = check_extension_support(device);

    bool swap_chain_adequate = false;
    if(extensions_supported){
        SwapChainSupportDetails swap_chain_details = query_swapchain_support(device);
        swap_chain_adequate = !swap_chain_details.formats.empty() && !swap_chain_details.present_modes.empty();
    }

    return indecies.is_complete() &&
    extensions_supported &&
    swap_chain_adequate &&
    device_properties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

bool Device::check_extension_support(VkPhysicalDevice device){
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

    std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

    for(const auto& extension : available_extensions){
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

Device::QueueFamilyIndecies Device::find_queue_families(VkPhysicalDevice device){
    QueueFamilyIndecies indecies;

    uint32_t queue_family = 0;
    vkGetPhysicalDeviceQueueFamilyProperties2(device, &queue_family, nullptr);

    std::vector<VkQueueFamilyProperties2> queue_families;
    vkGetPhysicalDeviceQueueFamilyProperties2(device, &queue_family, queue_families.data());

    size_t i = 0;
    for(const auto& queue_family:queue_families){
        if(queue_family.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indecies.graphics_family = i;
        }

        VkBool32 presentation_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentation_support);

        if(presentation_support){
            indecies.presentation_family = i;
        }

        if(indecies.is_complete()) break;

        i++;
    }

    return indecies;
}

Device::SwapChainSupportDetails Device::query_swapchain_support(VkPhysicalDevice device){
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &details.capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);

    if(format_count != 0){
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);
    
    if(present_mode_count != 0){
        details.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.present_modes.data());
    }

    return details;
}

void Device::create_surface(){
    window.create_surface(instance, &surface);
}

Device::~Device(){
    vkDestroyDevice(logical_device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

}
