#include "device.h"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
namespace FRI{

Device::Device(const std::string& application_name){
    create_instance(application_name);
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

    // Specify Queues We Want To Interface With
    float queue_priority = 1.0f;// Queue priority from 0.0 - 1.0 for command buffer scheduling
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = indecies.graphics_family.value();
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    
    // Select GPU Features We Want To Use
    VkPhysicalDeviceFeatures device_features{};

    // Logical Device Creation Information
    VkDeviceCreateInfo device_create_info{};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pQueueCreateInfos = &queue_create_info;
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pEnabledFeatures = &device_features;
    device_create_info.enabledExtensionCount = 0;

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

    return device_properties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
    device_features.features.geometryShader && indecies.graphics_family.has_value();
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

        if(indecies.is_complete()) break;

        i++;
    }

    return indecies;
}

Device::~Device(){
    vkDestroyDevice(logical_device, nullptr);
    vkDestroyInstance(instance, nullptr);
}

}
