#include "graphics_pipeline.hpp"
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <stdexcept>

namespace FRI{


std::vector<char> Pipeline::parse_shader(const std::string& shader_source){
    std::ifstream file(shader_source, std::ios::ate | std::ios::binary);

    if(!file.is_open()){ throw std::runtime_error("Failed To Parse Shader" + shader_source); }

    size_t file_size = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);

    file.close();
    return buffer;
}

Pipeline::Pipeline(const std::string& vertex_shader_path, const std::string& fragment_shader_path){
    auto vertex_code = parse_shader(vertex_shader_path);
    auto fragment_code = parse_shader(fragment_shader_path);

    fprintf(stdout,"Vertex Shader Code Size: %ld\n", vertex_code.size());
    fprintf(stdout,"Fragment Shader Code Size: %ld\n", fragment_code.size());
}

}
