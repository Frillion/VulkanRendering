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

Device::~Device(){
    vkDestroyInstance(instance, nullptr);
}

}
